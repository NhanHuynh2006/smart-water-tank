"""Phat lai du lieu that (06/10 16:31-16:37) vao ban SAO cua backend tren cong 8010.
Khong cham vao co so du lieu va broker that."""
import datetime as dt, json, os, shutil, sqlite3, subprocess, sys, time
import paho.mqtt.client as mqtt

D = os.environ.get("DEMO_DIR", "/tmp/wt-demo"); os.makedirs(D, exist_ok=True)
open(D + "/mosq.conf", "w").write("listener 18830 127.0.0.1\nallow_anonymous true\npersistence false\n")
SRC = "/home/nhanhuynh/Documents/IOT/IOT/backend/watertank.db"
DB = D + "/demo.db"
END = dt.datetime(2026, 10, 6, 16, 36, 59).timestamp()      # cuoi lich su
LIVE = (END, dt.datetime(2026, 10, 6, 16, 37, 20).timestamp())  # phat song

shutil.copy(SRC, DB)
c = sqlite3.connect(DB)
live = c.execute("SELECT * FROM telemetry WHERE recv_ts>? AND recv_ts<=? AND replay=0 ORDER BY recv_ts", LIVE).fetchall()
cols = [x[1] for x in c.execute("pragma table_info(telemetry)")]
t0 = time.time() + 8
off = t0 - END
for t, tc in (("telemetry", ("recv_ts",)), ("pump_events", ("recv_ts",)), ("faults", ("recv_ts",)),
              ("availability", ("recv_ts",)), ("commands", ("sent_ts", "ack_ts"))):
    c.execute(f"DELETE FROM {t} WHERE {tc[0]}>?", (END,))
    for col in tc:
        c.execute(f"UPDATE {t} SET {col}={col}+? WHERE {col} IS NOT NULL", (off,))
c.execute("UPDATE telemetry SET ts=ts+?, ts_ms=ts_ms+? WHERE ts>1600000000", (int(off), int(off * 1000)))
for t in ("pump_events", "faults"):
    c.execute(f"UPDATE {t} SET ts=ts+? WHERE ts>1600000000", (int(off),))
c.commit(); c.close()

broker = subprocess.Popen(["mosquitto", "-c", D + "/mosq.conf"])
time.sleep(1)
env = dict(os.environ, DB_PATH=DB, MQTT_PORT="18830")
env.pop("PYTHONPATH", None)
backend = subprocess.Popen([sys.executable, "-c",
    "import threading,uvicorn,app; app.init_db(); app.rebuild_leak_baseline();"
    "threading.Thread(target=app.mqtt_thread,daemon=True).start();"
    "uvicorn.run(app.app,host='127.0.0.1',port=8010,log_level='warning')"],
    cwd="/home/nhanhuynh/Documents/IOT/IOT/backend", env=env)
time.sleep(4)

m = mqtt.Client(client_id="demo-replay")
m.connect("127.0.0.1", 18830); m.loop_start()
R = "wt/lab1/esp32-01"
m.publish(R + "/status", json.dumps({"online": True}), qos=1, retain=True)
m.publish(R + "/state/pump", json.dumps({"dev": "esp32-01", "pump": True, "state": "FILLING"}), qos=1, retain=True)
while time.time() < t0: time.sleep(0.2)
i = 0
deadline = time.time() + float(sys.argv[1] if len(sys.argv) > 1 else 120)
while time.time() < deadline:
    row = dict(zip(cols, live[min(i, len(live) - 1)]))
    mv = row["current_mv"] or 0
    d = {"dev": "esp32-01", "ts": int(time.time()), "ts_ms": int(time.time() * 1000), "seq": 5000 + i,
         "level_pct": row["level_pct"], "level_cm": row["level_cm"], "level_ok": bool(row["level_ok"]),
         "level_model_cm": row["level_cm"], "blind_ms": 0, "level_rate_cms": 0.045,
         "flow_lpm": row["flow_lpm"], "volume_l": row["volume_l"], "volume_today_l": row["volume_today_l"],
         "flow_out_lpm": row["flow_out_lpm"], "volume_out_l": row["volume_out_l"],
         "volume_out_today_l": row["volume_out_today_l"], "flow_src": "level", "flow_ok": True, "flow_out_ok": True,
         "sens_in_lpm": 0.33, "sens_out_lpm": 0.0,
         "pump": bool(row["pump"]), "state": row["state"], "mode": row["mode"],
         "current_mv": mv, "current_ma": round(mv * 8.11, 1), "pump_health": "ok" if row["pump"] else "idle",
         "float_max": False, "float_min": False, "fault": "", "rssi": row["rssi"] or -48, "ctl_gap_ms": 201}
    m.publish(R + "/telemetry", json.dumps(d))
    i += 1
    time.sleep(1)
backend.terminate(); broker.terminate()
