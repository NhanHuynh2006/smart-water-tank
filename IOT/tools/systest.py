#!/usr/bin/env python3
"""Kiem thu he thong tu dong, chay tren may chu khi ESP32 dang noi mang.

    cd IOT/backend
    env -u PYTHONPATH .venv/bin/python ../tools/systest.py            # moi bai tru mat mang
    SUDO_PASS=... env -u PYTHONPATH .venv/bin/python ../tools/systest.py --outage

Cac bai:
  T1 ket noi     thiet bi truc tuyen, telemetry 1 Hz, du truong, status giu lai
  T2 bao mat     xem khong can dang nhap; lenh khong phien -> 401; sai mat khau -> 401
  T3 lenh        40 lenh doi che do, do khu hoi, khong lenh nao mat xac nhan
  T4 tu choi     bat bom o che do tu dong; bat lai trong thoi gian nghi; xoa loi khi khong co loi
  T5 bom tay     bat 8 s: co dong dien, nhan suc khoe; tat: dong ve 0
  T6 mat mang    (--outage) chan IP thiet bi 90 s bang iptables, kiem tra phat lai
Ket qua ghi vao IOT/docs/systest_<gio>.json.
"""
import argparse, json, os, statistics as st, subprocess, sys, threading, time
from datetime import datetime
import urllib.request, urllib.error, http.cookiejar
import paho.mqtt.client as mqtt

HERE = os.path.dirname(os.path.abspath(__file__))
BASE = "http://127.0.0.1:8000"
PW_FILE = os.path.expanduser("~/.cache/water-tank/admin_password")
cj = http.cookiejar.CookieJar()
opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(cj))
log, lock = [], threading.Lock()
results = {}


def http(method, path, body=None, auth=True):
    op = opener if auth else urllib.request.build_opener()
    req = urllib.request.Request(BASE + path, method=method,
                                 data=json.dumps(body).encode() if body is not None else None,
                                 headers={"Content-Type": "application/json"})
    try:
        with op.open(req, timeout=5) as r:
            return r.status, json.loads(r.read() or b"null")
    except urllib.error.HTTPError as e:
        return e.code, None


def latest():
    return http("GET", "/api/latest")[1] or {}


def cmd(action, value, wait=3.0):
    code, r = http("POST", "/api/command", {"action": action, "value": value})
    if code != 200:
        return {"http": code}
    cid, t0 = r["cmd_id"], time.time()
    while time.time() - t0 < wait:
        for c in http("GET", "/api/commands?limit=60")[1]:
            if c["cmd_id"] == cid and c["status"] != "pending":
                return c
        time.sleep(0.1)
    return {"cmd_id": cid, "status": "pending"}


def on_msg(_c, _u, m):
    try:
        p = json.loads(m.payload.decode())
    except Exception:
        return
    with lock:
        log.append((time.time(), m.topic, p))


def check(name, ok, detail):
    results[name] = {"pass": bool(ok), **detail}
    print(f"[{'PASS' if ok else 'FAIL'}] {name}: {detail}")


def t1():
    t0 = time.time(); time.sleep(10)
    with lock:
        tel = [p for t, tp, p in log if t >= t0 and tp.endswith("/telemetry") and not p.get("replay")]
        status = [p for t, tp, p in log if tp.endswith("/status")]
    need = ["seq", "ts_ms", "level_pct", "level_ok", "pump", "state", "mode", "current_ma",
            "pump_health", "flow_lpm", "flow_out_lpm", "volume_l", "fault", "ctl_gap_ms"]
    missing = sorted({k for p in tel for k in need if k not in p})
    seqs = [p["seq"] for p in tel]
    gaps = sum(1 for a, b in zip(seqs, seqs[1:]) if b != a + 1)
    d = latest()
    check("T1 connectivity", d.get("online") and len(tel) >= 8 and not missing and gaps == 0
          and status and status[-1].get("online"),
          {"messages_10s": len(tel), "seq_gaps": gaps, "missing_fields": missing,
           "retained_status": status[-1] if status else None, "age_s": d.get("age_s")})


def t2():
    pub = urllib.request.build_opener()
    v = pub.open(BASE + "/", timeout=5).status
    c1, _ = http("POST", "/api/command", {"action": "mode", "value": True}, auth=False)
    c2, _ = http("POST", "/api/login", {"password": "sai-mat-khau"}, auth=False)
    c3, _ = http("POST", "/api/login", {"password": open(PW_FILE).read().strip()})
    check("T2 security", v == 200 and c1 == 401 and c2 == 401 and c3 == 200,
          {"view": v, "command_without_session": c1, "wrong_password": c2, "login": c3})


def t3(n=20):
    rtt, lost = [], 0
    for i in range(n):
        for val in (False, True):
            r = cmd("mode", val)
            if r.get("status") == "accepted":
                rtt.append(r["latency_ms"])
            else:
                lost += 1
            time.sleep(1.0)
    rtt.sort()
    check("T3 commands", lost == 0, {"n": len(rtt), "lost": lost,
          "median_ms": round(st.median(rtt), 1), "p95_ms": round(rtt[int(0.95 * (len(rtt) - 1))], 1),
          "min_ms": round(rtt[0], 1), "max_ms": round(rtt[-1], 1)})


def t4():
    a = cmd("pump", True)                       # che do tu dong
    cmd("mode", False); time.sleep(1)
    if latest().get("data", {}).get("pump"):
        cmd("pump", False)
    time.sleep(1)
    b1 = cmd("pump", True); time.sleep(4); cmd("pump", False); time.sleep(1)
    b2 = cmd("pump", True)                      # trong 20 s nghi
    c = cmd("clear_fault", None)
    cmd("mode", True)
    ok = (a.get("reason") == "auto_mode_active" and b2.get("reason") == "min_off_time"
          and c.get("reason") == "no_active_fault")
    check("T4 rejections", ok, {"pump_in_auto": a.get("reason"), "restart_during_rest": b2.get("reason"),
                                "clear_without_fault": c.get("reason"), "first_manual_start": b1.get("status")})


def t5():
    cmd("mode", False); time.sleep(22)          # qua thoi gian nghi toi thieu
    r = cmd("pump", True)
    t0 = time.time(); time.sleep(8)
    with lock:
        on = [p for t, tp, p in log if t >= t0 + 2 and tp.endswith("/telemetry") and p.get("pump")]
    cmd("pump", False); t1 = time.time(); time.sleep(3)
    with lock:
        off = [p for t, tp, p in log if t >= t1 + 1 and tp.endswith("/telemetry")]
    cmd("mode", True)
    i_on = [p.get("current_ma", 0) for p in on]
    check("T5 manual pump", r.get("status") == "accepted" and i_on and min(i_on) > 100
          and all(p.get("current_ma", 1) == 0 for p in off),
          {"accepted": r.get("status"), "current_on_ma": [round(x) for x in i_on],
           "health_on": sorted({p.get("pump_health") for p in on}),
           "current_off_ma": [p.get("current_ma") for p in off]})


def t6(ip, seconds):
    pw = os.environ.get("SUDO_PASS", "")
    def ipt(op):
        for rule in (["INPUT", "-s", ip], ["OUTPUT", "-d", ip]):
            subprocess.run(["sudo", "-S", "iptables", op, *rule, "-j", "DROP"],
                           input=(pw + "\n").encode(), check=True, capture_output=True)
    t_cut = time.time(); ipt("-I")
    try:
        time.sleep(seconds)
    finally:
        ipt("-D")
    t_back = time.time(); time.sleep(40)
    with lock:
        tel = [(t, p) for t, tp, p in log if tp.endswith("/telemetry")]
        lwt = [t for t, tp, p in log if tp.endswith("/status") and p.get("online") is False and t > t_cut]
    before = [p["seq"] for t, p in tel if t < t_back and not p.get("replay")]
    after = [p for t, p in tel if t > t_back and not p.get("replay")]
    rep = [p["seq"] for t, p in tel if p.get("replay") and t > t_back]
    want = set(range(before[-1] + 1, after[0]["seq"])) if before and after else set()
    check("T6 outage", after and len(rep) > 0 and (after[0].get("ctl_gap_ms") or 0) < 2000,
          {"outage_s": seconds, "lwt_after_s": round(lwt[0] - t_cut, 1) if lwt else None,
           "replayed": len(rep), "missing": len(want - set(rep)),
           "ctl_gap_ms": after[0].get("ctl_gap_ms") if after else None})


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--outage", action="store_true")
    ap.add_argument("--ip", default="10.87.28.13")
    ap.add_argument("--seconds", type=int, default=90)
    a = ap.parse_args()
    c = mqtt.Client(client_id=f"systest-{os.getpid()}")
    c.username_pw_set(os.getenv("MQTT_USER", "backend"), os.getenv("MQTT_PASS", "backend_pass"))
    c.on_message = on_msg
    c.on_connect = lambda cl, u, f, rc: cl.subscribe("wt/#", 1)
    c.connect("localhost", 1883, 30); c.loop_start()
    http("POST", "/api/login", {"password": open(PW_FILE).read().strip()})
    for f in (t1, t2, t3, t4, t5):
        try:
            f()
        except Exception as e:
            check(f.__name__, False, {"error": repr(e)})
    if a.outage:
        t6(a.ip, a.seconds)
    out = os.path.join(HERE, "..", "docs", f"systest_{datetime.now():%Y%m%d_%H%M%S}.json")
    json.dump(results, open(out, "w"), indent=1, ensure_ascii=False)
    print(f"\n{sum(r['pass'] for r in results.values())}/{len(results)} PASS · {os.path.relpath(out)}")
