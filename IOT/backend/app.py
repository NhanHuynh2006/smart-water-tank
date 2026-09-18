"""
Dich vu nen cho he thong bon nuoc thong minh.

Ba thanh phan chay song song trong mot tien trinh:
  1. Bo thu nhan MQTT      (luong rieng)
  2. May chu API FastAPI
  3. Bo phat hien ro ri theo duong nen truot (goi moi khi co ban tin moi)

Chay:  python app.py
"""
import json
import math
import os
import sqlite3
import threading
import time
import uuid
from contextlib import contextmanager

import paho.mqtt.client as mqtt
from fastapi import FastAPI, HTTPException, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from pydantic import BaseModel

# ----------------------------------------------------------------------
# Cau hinh (co the doi bang bien moi truong)
# ----------------------------------------------------------------------
MQTT_HOST = os.getenv("MQTT_HOST", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_USER = os.getenv("MQTT_USER", "backend")
MQTT_PASS = os.getenv("MQTT_PASS", "backend_pass")
SITE_ID = os.getenv("SITE_ID", "lab1")
DEVICE_ID = os.getenv("DEVICE_ID", "esp32-01")
DB_PATH = os.getenv("DB_PATH", "watertank.db")
STALE_SECONDS = 5

ROOT = f"wt/{SITE_ID}/{DEVICE_ID}"
T_TELEMETRY = f"{ROOT}/telemetry"
T_PUMP = f"{ROOT}/state/pump"
T_FAULT = f"{ROOT}/event/fault"
T_STATUS = f"{ROOT}/status"
T_CMD = f"{ROOT}/cmd"
T_ACK = f"{ROOT}/cmd/ack"

# ----------------------------------------------------------------------
# Co so du lieu
# ----------------------------------------------------------------------
_db_lock = threading.Lock()


@contextmanager
def db():
    conn = sqlite3.connect(DB_PATH, timeout=10)
    conn.row_factory = sqlite3.Row
    try:
        yield conn
        conn.commit()
    finally:
        conn.close()


SCHEMA = """
CREATE TABLE IF NOT EXISTS telemetry (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  dev TEXT NOT NULL,
  ts INTEGER,                -- dau thoi gian thiet bi sinh, do phan giai GIAY
  ts_ms INTEGER,             -- nhu tren nhung do phan giai MILI GIAY (E7)
  recv_ts REAL NOT NULL,     -- dau thoi gian may chu nhan duoc
  seq INTEGER,               -- KHONG danh chi muc, tranh bung no luc luong
  level_pct REAL, level_cm REAL, level_ok INTEGER,
  flow_lpm REAL, volume_l REAL, volume_today_l REAL,
  flow_out_lpm REAL, volume_out_l REAL, volume_out_today_l REAL,
  pump INTEGER, state TEXT, mode TEXT,
  current_mv REAL, float_max INTEGER, float_src INTEGER,
  fault TEXT, rssi INTEGER, replay INTEGER DEFAULT 0
);
CREATE INDEX IF NOT EXISTS idx_tel_recv ON telemetry(recv_ts);

CREATE TABLE IF NOT EXISTS faults (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  dev TEXT, code TEXT, ts INTEGER, recv_ts REAL,
  level_pct REAL, flow_lpm REAL
);

CREATE TABLE IF NOT EXISTS pump_events (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  dev TEXT, pump INTEGER, state TEXT, ts INTEGER, recv_ts REAL
);

CREATE TABLE IF NOT EXISTS commands (
  cmd_id TEXT PRIMARY KEY,
  action TEXT, value TEXT,
  sent_ts REAL, ack_ts REAL,
  status TEXT, reason TEXT, latency_ms REAL
);

CREATE TABLE IF NOT EXISTS availability (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  dev TEXT, online INTEGER, recv_ts REAL
);

CREATE TABLE IF NOT EXISTS config (
  key TEXT PRIMARY KEY, value TEXT
);
"""

DEFAULT_CONFIG = {
    "level_low_pct": "30",
    "level_high_pct": "80",
    "level_overflow_pct": "95",
    "min_on_s": "10",
    "min_off_s": "20",
}


def init_db():
    with db() as c:
        c.executescript(SCHEMA)
        # Di tru cho co so du lieu tao truoc khi co truong mili giay.
        cols = {r[1] for r in c.execute("PRAGMA table_info(telemetry)")}
        if "ts_ms" not in cols:
            c.execute("ALTER TABLE telemetry ADD COLUMN ts_ms INTEGER")
            print("[DB] da them cot telemetry.ts_ms")
        for col in ("flow_out_lpm", "volume_out_l", "volume_out_today_l"):
            if col not in cols:
                c.execute(f"ALTER TABLE telemetry ADD COLUMN {col} REAL")
                print(f"[DB] da them cot telemetry.{col}")
        for k, v in DEFAULT_CONFIG.items():
            c.execute("INSERT OR IGNORE INTO config(key,value) VALUES(?,?)", (k, v))


# ----------------------------------------------------------------------
# Bo phat hien ro ri theo duong nen truot (EWMA tren 48 khe 30 phut)
# ----------------------------------------------------------------------
class LeakDetector:
    ALPHA = 0.2
    KAPPA = 3.0
    SLOTS = 48

    def __init__(self):
        self.mu = [0.0] * self.SLOTS
        self.var = [0.0] * self.SLOTS
        self.seen = [0] * self.SLOTS
        self.slot_start_volume = None
        self.current_slot = None
        self.consecutive = 0
        self.last_alert = 0.0

    @staticmethod
    def slot_of(ts: float) -> int:
        lt = time.localtime(ts)
        return (lt.tm_hour * 60 + lt.tm_min) // 30

    def update(self, ts: float, volume_l: float):
        """Goi moi khi co ban tin. Tra ve chuoi canh bao hoac None."""
        slot = self.slot_of(ts)
        if self.current_slot is None:
            self.current_slot = slot
            self.slot_start_volume = volume_l
            return None
        if slot == self.current_slot:
            return None

        used = max(0.0, volume_l - self.slot_start_volume)
        k = self.current_slot
        alert = None

        if self.seen[k] >= 3:
            sigma = math.sqrt(max(self.var[k], 1e-9))
            if used > self.mu[k] + self.KAPPA * sigma:
                self.consecutive += 1
                if self.consecutive >= 2 and time.time() - self.last_alert > 1800:
                    alert = (f"tieu thu {used:.2f} L trong khe {k}, "
                             f"vuot nguong {self.mu[k] + self.KAPPA * sigma:.2f} L")
                    self.last_alert = time.time()
            else:
                self.consecutive = 0

        d = used - self.mu[k]
        self.mu[k] += self.ALPHA * d
        self.var[k] = (1 - self.ALPHA) * (self.var[k] + self.ALPHA * d * d)
        self.seen[k] += 1

        self.current_slot = slot
        self.slot_start_volume = volume_l
        return alert


leak = LeakDetector()

# ----------------------------------------------------------------------
# Trang thai moi nhat giu trong bo nho
# ----------------------------------------------------------------------
latest = {"data": None, "recv_ts": 0.0, "online": False}
pending = {}
_state_lock = threading.Lock()
last_pump_state = None

# ----------------------------------------------------------------------
# Bo thu nhan MQTT
# ----------------------------------------------------------------------
client = mqtt.Client(client_id=f"backend-{uuid.uuid4().hex[:6]}")


def on_connect(cli, userdata, flags, rc):
    print(f"[MQTT] ket noi rc={rc}")
    for t in (T_TELEMETRY, T_PUMP, T_FAULT, T_STATUS, T_ACK):
        cli.subscribe(t, qos=1 if t != T_TELEMETRY else 0)


def on_message(cli, userdata, msg):
    global last_pump_state
    now = time.time()
    try:
        d = json.loads(msg.payload.decode())
    except Exception:
        return

    if msg.topic == T_TELEMETRY:
        with _state_lock:
            latest["data"] = d
            latest["recv_ts"] = now
            latest["online"] = True
        with _db_lock, db() as c:
            c.execute(
                """INSERT INTO telemetry(dev,ts,ts_ms,recv_ts,seq,level_pct,level_cm,level_ok,
                   flow_lpm,volume_l,volume_today_l,
                   flow_out_lpm,volume_out_l,volume_out_today_l,
                   pump,state,mode,current_mv,
                   float_max,float_src,fault,rssi,replay)
                   VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)""",
                (d.get("dev"), d.get("ts"), d.get("ts_ms"), now, d.get("seq"),
                 d.get("level_pct"), d.get("level_cm"), int(bool(d.get("level_ok"))),
                 d.get("flow_lpm"), d.get("volume_l"), d.get("volume_today_l"),
                 d.get("flow_out_lpm"), d.get("volume_out_l"), d.get("volume_out_today_l"),
                 int(bool(d.get("pump"))), d.get("state"), d.get("mode"),
                 d.get("current_mv"), int(bool(d.get("float_max"))),
                 int(bool(d.get("float_src"))), d.get("fault"), d.get("rssi"),
                 int(bool(d.get("replay")))))
        if not d.get("replay") and d.get("volume_l") is not None:
            a = leak.update(now, float(d["volume_l"]))
            if a:
                print(f"[LEAK] {a}")
                with _db_lock, db() as c:
                    c.execute("INSERT INTO faults(dev,code,ts,recv_ts,level_pct,flow_lpm)"
                              " VALUES(?,?,?,?,?,?)",
                              (d.get("dev"), "LEAK_BASELINE", d.get("ts"), now,
                               d.get("level_pct"), d.get("flow_lpm")))

    elif msg.topic == T_PUMP:
        p = int(bool(d.get("pump")))
        if p != last_pump_state:
            last_pump_state = p
            with _db_lock, db() as c:
                c.execute("INSERT INTO pump_events(dev,pump,state,ts,recv_ts)"
                          " VALUES(?,?,?,?,?)",
                          (d.get("dev"), p, d.get("state"), d.get("ts"), now))

    elif msg.topic == T_FAULT:
        print(f"[FAULT] {d.get('code')}")
        with _db_lock, db() as c:
            c.execute("INSERT INTO faults(dev,code,ts,recv_ts,level_pct,flow_lpm)"
                      " VALUES(?,?,?,?,?,?)",
                      (d.get("dev"), d.get("code"), d.get("ts"), now,
                       d.get("level_pct"), d.get("flow_lpm")))

    elif msg.topic == T_STATUS:
        online = bool(d.get("online"))
        with _state_lock:
            latest["online"] = online
        with _db_lock, db() as c:
            c.execute("INSERT INTO availability(dev,online,recv_ts) VALUES(?,?,?)",
                      (DEVICE_ID, int(online), now))

    elif msg.topic == T_ACK:
        cid = d.get("cmd_id")
        with _state_lock:
            sent = pending.pop(cid, None)
        lat = (now - sent) * 1000 if sent else None
        with _db_lock, db() as c:
            c.execute("""UPDATE commands SET ack_ts=?,status=?,reason=?,latency_ms=?
                         WHERE cmd_id=?""",
                      (now, d.get("status"), d.get("reason"), lat, cid))
        print(f"[ACK] {cid} {d.get('status')} {d.get('reason') or ''} "
              f"{'%.1f ms' % lat if lat else ''}")


def mqtt_thread():
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.on_connect = on_connect
    client.on_message = on_message
    while True:
        try:
            client.connect(MQTT_HOST, MQTT_PORT, keepalive=30)
            client.loop_forever()
        except Exception as e:
            print(f"[MQTT] loi {e}, thu lai sau 3 giay")
            time.sleep(3)


# ----------------------------------------------------------------------
# API
# ----------------------------------------------------------------------
app = FastAPI(title="Smart Water Tank API", version="1.0")
app.add_middleware(CORSMiddleware, allow_origins=["*"],
                   allow_methods=["*"], allow_headers=["*"])


class Command(BaseModel):
    action: str
    value: object = None


@app.get("/")
def index():
    p = os.path.join(os.path.dirname(__file__), "..", "dashboard", "index.html")
    if os.path.exists(p):
        # no-cache bat trinh duyet hoi lai may chu moi lan tai trang. Van re vi
        # ETag van hoat dong: file khong doi thi tra 304 rong. Thieu header nay
        # thi trinh duyet TU SUY DOAN thoi han va phuc vu ban cu da luu, khien
        # sua dashboard xong tai lai van thay giao dien cu.
        return FileResponse(p, headers={"Cache-Control": "no-cache"})
    return {"msg": "dashboard/index.html khong tim thay"}


@app.get("/api/health")
def health():
    return {"backend": "ok", "mqtt_connected": client.is_connected(),
            "broker": f"{MQTT_HOST}:{MQTT_PORT}", "db": DB_PATH}


@app.get("/api/latest")
def get_latest():
    with _state_lock:
        d = latest["data"]
        recv = latest["recv_ts"]
        online = latest["online"]
        pend = list(pending.keys())
    age = (time.time() - recv) if recv else None
    return {"data": d, "age_s": age,
            "stale": (age is None or age > STALE_SECONDS),
            "online": online and age is not None and age <= STALE_SECONDS,
            "pending_commands": pend}


@app.get("/api/telemetry")
def get_telemetry(minutes: int = Query(10, ge=1, le=1440),
                  limit: int = Query(2000, ge=1, le=20000)):
    since = time.time() - minutes * 60
    with db() as c:
        # Lay N ban tin MOI NHAT roi moi sap xep tang dan de ve.
        # Neu sap xep tang dan roi moi LIMIT thi khi so ban tin trong cua so
        # vuot qua limit, truy van tra ve doan CU NHAT va bieu do dung yen o
        # qua khu trong khi cot trang thai van cap nhat, hai ben lech nhau.
        rows = c.execute(
            """SELECT * FROM (
                 SELECT recv_ts,ts,level_pct,flow_lpm,flow_out_lpm,volume_l,pump,state,level_ok
                 FROM telemetry WHERE recv_ts>=? ORDER BY recv_ts DESC LIMIT ?
               ) ORDER BY recv_ts ASC""",
            (since, limit)).fetchall()
    return [dict(r) for r in rows]


@app.get("/api/volume/daily")
def volume_daily(days: int = Query(7, ge=1, le=90)):
    since = time.time() - days * 86400
    with db() as c:
        rows = c.execute(
            """SELECT date(recv_ts,'unixepoch','localtime') AS day,
                      MAX(volume_l)-MIN(volume_l) AS used_l
               FROM telemetry WHERE recv_ts>=? GROUP BY day ORDER BY day""",
            (since,)).fetchall()
    return [dict(r) for r in rows]


@app.get("/api/faults")
def get_faults(limit: int = 50):
    with db() as c:
        rows = c.execute("SELECT * FROM faults ORDER BY id DESC LIMIT ?",
                         (limit,)).fetchall()
    return [dict(r) for r in rows]


@app.get("/api/events")
def get_events(limit: int = 100):
    with db() as c:
        rows = c.execute("SELECT * FROM pump_events ORDER BY id DESC LIMIT ?",
                         (limit,)).fetchall()
    return [dict(r) for r in rows]


@app.get("/api/commands")
def get_commands(limit: int = 30):
    with db() as c:
        rows = c.execute("SELECT * FROM commands ORDER BY sent_ts DESC LIMIT ?",
                         (limit,)).fetchall()
    return [dict(r) for r in rows]


@app.post("/api/command")
def post_command(cmd: Command):
    if cmd.action not in ("mode", "pump", "clear_fault", "reset_volume"):
        raise HTTPException(400, "hanh dong khong duoc ho tro")
    cid = uuid.uuid4().hex[:8]
    now = time.time()
    payload = {"cmd_id": cid, "action": cmd.action,
               "value": cmd.value, "ts": int(now)}
    with _db_lock, db() as c:
        c.execute("INSERT INTO commands(cmd_id,action,value,sent_ts,status)"
                  " VALUES(?,?,?,?,?)",
                  (cid, cmd.action, json.dumps(cmd.value), now, "pending"))
    with _state_lock:
        pending[cid] = now
    client.publish(T_CMD, json.dumps(payload), qos=1)
    return {"cmd_id": cid, "status": "pending"}


@app.get("/api/config")
def get_config():
    with db() as c:
        rows = c.execute("SELECT key,value FROM config").fetchall()
    return {r["key"]: r["value"] for r in rows}


@app.put("/api/config")
def put_config(cfg: dict):
    with _db_lock, db() as c:
        for k, v in cfg.items():
            c.execute("INSERT INTO config(key,value) VALUES(?,?) "
                      "ON CONFLICT(key) DO UPDATE SET value=excluded.value",
                      (k, str(v)))
    return get_config()


@app.get("/api/stats/latency")
def stats_latency(minutes: int = 10):
    since = time.time() - minutes * 60
    with db() as c:
        # Do phan giai mili giay khi thiet bi co gui ts_ms.
        fine = c.execute(
            """SELECT (recv_ts*1000.0 - ts_ms) AS lat FROM telemetry
               WHERE recv_ts>=? AND ts_ms IS NOT NULL AND ts_ms>0 AND replay=0""",
            (since,)).fetchall()
        # Duong lui cho thiet bi cu chi gui "ts" theo giay.
        coarse = c.execute(
            """SELECT (recv_ts-ts)*1000 AS lat FROM telemetry
               WHERE recv_ts>=? AND ts IS NOT NULL AND ts>0 AND replay=0
                 AND (ts_ms IS NULL OR ts_ms=0)""",
            (since,)).fetchall()
        cmds = c.execute(
            "SELECT latency_ms FROM commands WHERE latency_ms IS NOT NULL "
            "AND sent_ts>=?", (since,)).fetchall()

    def stats(vals):
        vals = sorted(v for v in vals if v is not None)
        if not vals:
            return None
        n = len(vals)
        return {"n": n, "min": round(vals[0], 1),
                "median": round(vals[n // 2], 1),
                "p95": round(vals[int(n * 0.95) - 1 if n > 1 else 0], 1),
                "max": round(vals[-1], 1)}

    one_way = stats([r["lat"] for r in fine]) or stats([r["lat"] for r in coarse])
    resolution = "ms" if fine else ("s" if coarse else None)

    # command_ms la so CHINH cua thi nghiem E7. No do khu hoi bang DUY NHAT
    # dong ho may chu o ca hai dau, nen khong dinh sai lech dong bo.
    # telemetry_ms do mot chieu giua HAI dong ho khac nhau, nen sai so bi chan
    # duoi boi do chinh xac dong bo NTP cua ESP32 (thuong +-10..50 ms qua
    # Wi-Fi). Chi dung no de doi chieu, dung lay lam ket qua chinh.
    return {
        "primary": "command_ms",
        "command_ms": stats([r["latency_ms"] for r in cmds]),
        "command_note": "khu hoi, mot dong ho o ca hai dau, khong dinh lech NTP",
        "telemetry_ms": one_way,
        "telemetry_resolution": resolution,
        "telemetry_note": (
            "mot chieu giua hai dong ho; sai so bi chan duoi boi do chinh xac "
            "dong bo NTP cua thiet bi. Do phan giai 's' con them sai so luong "
            "tu hoa +-500 ms."),
    }


# ----------------------------------------------------------------------
if __name__ == "__main__":
    import uvicorn
    init_db()
    threading.Thread(target=mqtt_thread, daemon=True).start()
    print("Dashboard: http://localhost:8000/   |   API docs: http://localhost:8000/docs")
    uvicorn.run(app, host="0.0.0.0", port=8000, log_level="warning")
