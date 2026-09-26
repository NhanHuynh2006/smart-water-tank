#!/usr/bin/env python3
"""Thi nghiem tren ban: mat mang va do tre phat hien su co.

Chay tu thu muc IOT/backend de dung chung venv voi backend:

    cd IOT/backend
    env -u PYTHONPATH .venv/bin/python ../tools/bench.py outage            # tu tat mang bang tay
    env -u PYTHONPATH .venv/bin/python ../tools/bench.py outage --ip 10.60.119.13 --seconds 90
    env -u PYTHONPATH .venv/bin/python ../tools/bench.py fault             # gay loi bang tay, do tre
    env -u PYTHONPATH .venv/bin/python ../tools/bench.py level             # E1 hieu chuan muc

outage  Ghi moi ban tin MQTT, cat mang (bang iptables neu cho --ip, khong thi
        nhac nguoi lam bang tay), noi lai, roi phan tich: tre bao mat ket noi
        (di chuc), tre noi lai, so ban tin phat lai, so seq mat, bom co tu
        bat/tat trong luc mat mang khong, vong dieu khien co bi treo khong.
level   E1: nhap muc do bang thuoc, cong cu lay trung binh level_cm 20 s va
        tinh sai so, RMSE va he so LEVEL_CAL_A/B de dua vao config.h.
fault   Nhan Enter DUNG LUC gay loi (nhac phao, rut day cam bien, nhac ong hut
        khoi nuoc...), cho ban tin event/fault dau tien va in do tre.

Ket qua ghi them vao IOT/docs/bench_<loai>_<gio>.json.
"""
import argparse, json, os, subprocess, sys, threading, time
from datetime import datetime

import paho.mqtt.client as mqtt

HERE = os.path.dirname(os.path.abspath(__file__))
DOCS = os.path.join(HERE, "..", "docs")
log, lock = [], threading.Lock()


def on_message(_c, _u, m):
    try:
        p = json.loads(m.payload.decode())
    except Exception:
        p = {"raw": m.payload.decode(errors="replace")}
    with lock:
        log.append((time.time(), m.topic, p))


def connect(host, port, user, pw):
    c = mqtt.Client(client_id=f"bench-{os.getpid()}")
    c.on_message = on_message
    # Dung tai khoan cua backend: broker bat ACL, tai khoan nay chi DOC duoc
    # nhanh wt/lab1/#, nen cong cu nay khong the vo tinh gui lenh.
    c.username_pw_set(user, pw)
    c.connect(host, port, 30)
    c.subscribe("wt/#", 1)
    c.loop_start()
    return c


def ts(t):
    return datetime.fromtimestamp(t).strftime("%H:%M:%S.%f")[:-3]


def since(t0, kind, pred=lambda p: True):
    with lock:
        for t, topic, p in log:
            if t >= t0 and topic.endswith(kind) and pred(p):
                return t, p
    return None, None


def iptables(ip, add):
    op = "-I" if add else "-D"
    for rule in (["INPUT", "-s", ip], ["OUTPUT", "-d", ip]):
        subprocess.run(["sudo", "iptables", op, rule[0], rule[1], rule[2], "-j", "DROP"], check=True)


def save(kind, res):
    path = os.path.join(DOCS, f"bench_{kind}_{datetime.now():%Y%m%d_%H%M%S}.json")
    with open(path, "w") as f:
        json.dump(res, f, indent=1, ensure_ascii=False)
    print(f"\nDa ghi {os.path.relpath(path)}")


def outage(a):
    print("Ghi nen 15 s de lay trang thai ban dau...")
    time.sleep(15)
    t_cut = time.time()
    if a.ip:
        iptables(a.ip, True)
        print(f"[{ts(t_cut)}] DA CAT mang voi {a.ip}. Cho {a.seconds} s...")
        try:
            time.sleep(a.seconds)
        finally:
            iptables(a.ip, False)
    else:
        input("TAT Wi-Fi / diem phat cua thiet bi ROI nhan Enter... ")
        t_cut = time.time()
        input(f"[{ts(t_cut)}] Dang mat mang. BAT LAI mang roi nhan Enter... ")
    t_back = time.time()
    print(f"[{ts(t_back)}] Da noi lai. Cho thiet bi quay ve va phat lai (toi da 120 s)...")

    deadline = t_back + 120
    while time.time() < deadline:
        t_live, _ = since(t_back, "/telemetry", lambda p: not p.get("replay"))
        if t_live and time.time() - t_live > 10:
            break
        time.sleep(1)

    with lock:
        snap = list(log)
    tel = [(t, p) for t, tp, p in snap if tp.endswith("/telemetry")]
    live_before = [(t, p) for t, p in tel if t < t_back and not p.get("replay")]
    replay = [p for t, p in tel if p.get("replay")]
    live_after = [(t, p) for t, p in tel if t >= t_back and not p.get("replay")]
    offline = next(((t, p) for t, tp, p in snap
                    if t >= t_cut and tp.endswith("/status") and p.get("online") is False), (None, None))
    online = next(((t, p) for t, tp, p in snap
                   if t >= t_back and tp.endswith("/status") and p.get("online") is True), (None, None))

    res = {"cut": ts(t_cut), "restore": ts(t_back), "outage_s": round(t_back - t_cut, 1)}
    if live_before:
        last_t, last_p = live_before[-1]
        res["last_live_seq"] = last_p.get("seq")
        res["state_before"] = f"{last_p.get('state')} pump={last_p.get('pump')} {last_p.get('level_pct')}%"
    res["lwt_after_cut_s"] = round(offline[0] - t_cut, 1) if offline[0] else None
    res["online_after_restore_s"] = round(online[0] - t_back, 1) if online[0] else None
    if live_after:
        first_t, first_p = live_after[0]
        res["first_live_after_restore_s"] = round(first_t - t_back, 1)
        res["first_live_seq"] = first_p.get("seq")
        res["ctl_gap_ms_during_outage"] = first_p.get("ctl_gap_ms")
        res["ring_drop"] = first_p.get("ring_drop")
        res["state_after"] = f"{first_p.get('state')} pump={first_p.get('pump')} {first_p.get('level_pct')}%"
    res["replayed"] = len(replay)
    if replay:
        seqs = sorted(p["seq"] for p in replay if "seq" in p)
        res["replay_seq"] = [seqs[0], seqs[-1]]
        # Thiet bi van tu dieu khien khi mat mang: chuyen trang thai bom
        # xuat hien NGAY TRONG cac mau phat lai.
        trans, prev = [], None
        for p in replay:
            k = (p.get("pump"), p.get("state"))
            if prev is not None and k != prev:
                trans.append(f"seq {p.get('seq')}: {prev[1]}->{k[1]} pump={k[0]} @ {p.get('level_pct')}%")
            prev = k
        res["pump_transitions_while_offline"] = trans
    if live_before and live_after:
        want = set(range(res["last_live_seq"] + 1, res["first_live_seq"]))
        got = {p["seq"] for p in replay if "seq" in p}
        res["missing_seq"] = len(want - got)
        res["expected_seq"] = len(want)

    print()
    for k, v in res.items():
        print(f"  {k:<32} {v}")
    save("outage", res)


def fault(a):
    input("San sang. Nhan Enter DUNG LUC ban gay loi... ")
    t0 = time.time()
    print(f"[{ts(t0)}] Da danh dau. Cho ban tin su co (toi da {a.timeout} s)...")
    while time.time() - t0 < a.timeout:
        t, p = since(t0, "/event/fault")
        if t:
            ps, pp = since(t0, "/state/pump", lambda q: q.get("pump") is False)
            res = {"mark": ts(t0), "code": p.get("code"), "delay_s": round(t - t0, 2),
                   "pump_health": p.get("pump_health"), "current_ma": p.get("current_ma"),
                   "level_pct": p.get("level_pct"),
                   "pump_off_after_s": round(ps - t0, 2) if ps else None}
            for k, v in res.items():
                print(f"  {k:<18} {v}")
            save("fault", res)
            return
        time.sleep(0.05)
    print("Het gio, khong co ban tin su co nao.")


def level(a):
    """E1: so muc nuoc thiet bi bao voi thuoc ke dung trong bon."""
    rows = []
    while True:
        r = input("\nDo muc nuoc bang thuoc tu DAY bon (cm), Enter trong de ket thuc: ").strip()
        if not r:
            break
        ref = float(r.replace(",", "."))
        t0 = time.time()
        print(f"  lay mau {a.window} s, dung dong nuoc va khong cham vao bon...")
        time.sleep(a.window)
        with lock:
            v = [p["level_cm"] for t, tp, p in log if t >= t0 and tp.endswith("/telemetry")
                 and not p.get("replay") and p.get("level_ok") and "level_cm" in p]
        if len(v) < 5:
            print("  qua it mau hop le, do lai"); continue
        m = sum(v) / len(v)
        sd = (sum((x - m) ** 2 for x in v) / (len(v) - 1)) ** 0.5
        rows.append({"ref_cm": ref, "dev_cm": round(m, 2), "sd_cm": round(sd, 3), "n": len(v),
                     "err_cm": round(m - ref, 2)})
        print(f"  thiet bi {m:.2f} cm (sd {sd:.3f}, n {len(v)}) · sai {m - ref:+.2f} cm")
    if not rows:
        return
    res = {"points": rows}
    if len(rows) >= 2:
        xs = [r["dev_cm"] for r in rows]; ys = [r["ref_cm"] for r in rows]
        mx, my = sum(xs) / len(xs), sum(ys) / len(ys)
        A = sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / sum((x - mx) ** 2 for x in xs)
        B = my - A * mx
        res["fit"] = {"LEVEL_CAL_A": round(A, 4), "LEVEL_CAL_B": round(B, 3)}
        res["rmse_cm"] = round((sum(r["err_cm"] ** 2 for r in rows) / len(rows)) ** 0.5, 2)
        res["max_abs_err_cm"] = max(abs(r["err_cm"]) for r in rows)
    print(json.dumps(res, indent=1))
    save("level", res)


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("mode", choices=["outage", "fault", "level"])
    ap.add_argument("--host", default="localhost")
    ap.add_argument("--port", type=int, default=1883)
    ap.add_argument("--user", default=os.getenv("MQTT_USER", "backend"))
    ap.add_argument("--password", default=os.getenv("MQTT_PASS", "backend_pass"))
    ap.add_argument("--ip", help="IP cua ESP32; co thi tu cat bang iptables (can sudo)")
    ap.add_argument("--seconds", type=int, default=90)
    ap.add_argument("--timeout", type=int, default=180)
    ap.add_argument("--window", type=int, default=20, help="level: so giay lay trung binh")
    a = ap.parse_args()
    connect(a.host, a.port, a.user, a.password)
    {"outage": outage, "fault": fault, "level": level}[a.mode](a)
