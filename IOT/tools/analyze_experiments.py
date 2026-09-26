"""Rut ket qua thi nghiem tu co so du lieu da ghi.

Chay:  cd IOT/backend && env -u PYTHONPATH .venv/bin/python ../tools/analyze_experiments.py
In ra ket qua dang bang, va ghi ban JSON vao IOT/docs/results.json de bao cao
va slide dung chung mot nguon so lieu.
"""
import json, sqlite3, statistics, time, os, sys

DB   = os.path.join(os.path.dirname(__file__), "..", "backend", "watertank.db")
OUT  = os.path.join(os.path.dirname(__file__), "..", "docs", "results.json")
AREA_CM2, H_MAX_CM = 100.0, 14.0          # tiet dien va muc lam viec 100%

# Doan chay bang cau hinh gan nhu cuoi cung (thung 14 cm, cong tu phan vi,
# loc alpha-beta). Truyen tham so de phan tich doan khac.
T0 = time.mktime(time.strptime(sys.argv[1] if len(sys.argv) > 1 else "2026-09-23 16:48", "%Y-%m-%d %H:%M"))
T1 = time.mktime(time.strptime(sys.argv[2] if len(sys.argv) > 2 else "2026-09-23 18:12", "%Y-%m-%d %H:%M"))

c = sqlite3.connect(DB); c.row_factory = sqlite3.Row
hm = lambda t: time.strftime("%H:%M:%S", time.localtime(t))
pct = lambda v, q: sorted(v)[min(len(v) - 1, int(q * len(v)))]
res = {"window": [hm(T0), hm(T1)]}

rows = c.execute("""SELECT recv_ts, level_cm, level_pct, level_ok, pump, mode, state,
                           volume_l, volume_out_l, flow_out_lpm, fault
                    FROM telemetry WHERE recv_ts BETWEEN ? AND ? AND replay = 0
                    ORDER BY recv_ts""", (T0, T1)).fetchall()
n = len(rows)
res["samples"] = n
res["level_ok_ratio"] = round(sum(r["level_ok"] for r in rows) / n, 4)
res["faults_in_window"] = c.execute(
    "SELECT COUNT(*) FROM faults WHERE recv_ts BETWEEN ? AND ?", (T0, T1)).fetchone()[0]

# ---------- E3/E4: chu ky bom tu dong ----------
cycles, i = [], 0
while i < n:
    if rows[i]["pump"] and rows[i]["mode"] == "AUTO" and (i == 0 or not rows[i-1]["pump"]):
        j = i
        while j < n and rows[j]["pump"]: j += 1
        if j >= n: break
        a, b = rows[i], rows[j]
        # dinh sau khi ngat: muc cao nhat trong 60 giay sau
        after = [r["level_pct"] for r in rows[j:] if r["recv_ts"] - b["recv_ts"] <= 60]
        # muc ngay truoc khi bat (trung binh 5 mau) de tranh mot mau nhieu
        before = [r["level_pct"] for r in rows[max(0, i-5):i]] or [a["level_pct"]]
        vin = (b["volume_l"] or 0) - (a["volume_l"] or 0)
        dh  = (b["level_cm"] - a["level_cm"])
        dur = b["recv_ts"] - a["recv_ts"]
        # Dong xa trong luc bom: KHONG lay tu truong flow_out cua telemetry
        # (trong cua so nay no con la cong thuc cu bi loi). Do doc lap bang do
        # doc muc nuoc luc bom TAT, 90 giay ngay truoc va 90 giay sau dinh, roi
        # lay trung binh — dong xa tang theo can bac hai chieu cao cot nuoc
        # (Torricelli) nen dau va cuoi lan bom khac nhau.
        def slope(seg):
            if len(seg) < 20: return None
            t0 = seg[0]["recv_ts"]
            xs = [r["recv_ts"] - t0 for r in seg]; ys = [r["level_cm"] for r in seg]
            mx, my = statistics.mean(xs), statistics.mean(ys)
            sxx = sum((x - mx) ** 2 for x in xs)
            return sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / sxx if sxx else None
        pre  = [r for r in rows[max(0, i-120):i] if not r["pump"] and a["recv_ts"] - r["recv_ts"] <= 90]
        tpk  = b["recv_ts"] + 20                     # bo 20 s dau: nuoc con trong ong
        post = [r for r in rows[j:] if not r["pump"] and tpk <= r["recv_ts"] <= tpk + 90]
        ds = [x for x in (slope(pre), slope(post)) if x is not None and x < 0]
        drain_lpm = -statistics.mean(ds) * AREA_CM2 * 60 / 1000 if ds else None
        v_ref = (dh * AREA_CM2 / 1000 + drain_lpm * dur / 60) if drain_lpm is not None else None
        cycles.append({
            "start": hm(a["recv_ts"]), "start_pct": round(statistics.mean(before), 1),
            "stop_pct": round(b["level_pct"], 1), "peak_after_pct": round(max(after), 1),
            "overshoot_pct": round(max(after) - b["level_pct"], 2),
            "duration_s": round(dur, 1),
            "rise_cm_s": round(dh / dur, 4) if dur else None,
            "drain_lpm": round(drain_lpm, 4) if drain_lpm is not None else None,
            "v_pumped_est_l": round(vin, 4),
            "v_reference_l": round(v_ref, 4) if v_ref is not None else None,
            "v_error_pct": round(100 * (vin - v_ref) / v_ref, 1) if v_ref and v_ref > 0.05 else None,
            # chu ky tu dong TRON VEN: bat duoi 35 %, ngat quanh nguong 70 %
            "complete": b["level_pct"] >= 69 and statistics.mean(before) < 35,
        })
        i = j
    else:
        i += 1
res["fill_cycles"] = cycles

# khoang nghi giua hai lan bat (chong bat tat lien tuc)
ons = [r["recv_ts"] for k, r in enumerate(rows) if r["pump"] and k and not rows[k-1]["pump"]]
offs = [r["recv_ts"] for k, r in enumerate(rows) if not r["pump"] and k and rows[k-1]["pump"]]
gaps = [b - a for a in offs for b in ons if b > a and all(not (a < x < b) for x in ons if x != b)]
res["pump_starts"] = len(ons)
res["pump_starts_per_hour"] = round(len(ons) / ((T1 - T0) / 3600), 2)
res["min_off_gap_s"] = round(min(gaps), 1) if gaps else None

# ---------- E7: do tre ----------
lat = [r[0] for r in c.execute(
    """SELECT latency_ms FROM commands WHERE latency_ms IS NOT NULL
       AND sent_ts >= strftime('%s','2026-09-23')""")]
if lat:
    res["command_rtt_ms"] = {"n": len(lat), "median": round(statistics.median(lat), 1),
                             "p95": round(pct(lat, .95), 1), "max": round(max(lat), 1)}
one = [r[0] for r in c.execute(
    """SELECT recv_ts*1000 - ts_ms FROM telemetry WHERE ts_ms > 0 AND replay = 0
       AND recv_ts BETWEEN ? AND ?""", (T0, T1))]
if one:
    res["telemetry_one_way_ms"] = {"n": len(one), "median": round(statistics.median(one), 1),
                                   "p95": round(pct(one, .95), 1),
                                   "note": "hai dong ho khac nhau, chi de tham khao"}

# ---------- E5: do tre phat hien su co cam bien, tu cac su co da ghi ----------
det = []
for f in c.execute("SELECT recv_ts, code FROM faults WHERE code = 'SENSOR_TIMEOUT' ORDER BY recv_ts"):
    prev = c.execute("""SELECT recv_ts FROM telemetry WHERE recv_ts < ? AND level_ok = 1
                        ORDER BY recv_ts DESC LIMIT 1""", (f["recv_ts"],)).fetchone()
    if prev: det.append(round(f["recv_ts"] - prev[0], 1))
res["sensor_timeout_delay_s"] = det

json.dump(res, open(OUT, "w"), indent=1, ensure_ascii=False)

print(f"Cua so {res['window'][0]} -> {res['window'][1]} · {n} mau")
print(f"  level_ok {100*res['level_ok_ratio']:.1f}% · su co trong cua so: {res['faults_in_window']}")
print(f"  bom bat {res['pump_starts']} lan ({res['pump_starts_per_hour']}/gio) · nghi ngan nhat {res['min_off_gap_s']} s")
full = [k for k in cycles if k["complete"]]
print(f"  chu ky bom tu dong tron ven: {len(full)}/{len(cycles)}")
for k in full:
    vr = f"{k['v_reference_l']:.3f}" if k['v_reference_l'] is not None else "  -  "
    print(f"   {k['start']}  {k['start_pct']:5.1f}% -> {k['stop_pct']:5.1f}%  dinh {k['peak_after_pct']:5.1f}%"
          f"  vot {k['overshoot_pct']:+5.2f}%  {k['duration_s']:6.1f} s  xa {k['drain_lpm']} L/ph"
          f"  V bom {k['v_pumped_est_l']:.3f}  V chuan {vr}  sai {k['v_error_pct']}%")
def summ(key):
    v = [k[key] for k in full if k[key] is not None]
    return (round(statistics.mean(v), 2), round(min(v), 2), round(max(v), 2), len(v)) if v else None
res["summary"] = {k: summ(k) for k in ("stop_pct", "overshoot_pct", "duration_s", "v_error_pct", "drain_lpm")}
print("  tom tat (trung binh, nho nhat, lon nhat, n):")
for k, v in res["summary"].items(): print(f"   {k:<14} {v}")
json.dump(res, open(OUT, "w"), indent=1, ensure_ascii=False)
if "command_rtt_ms" in res: print("  lenh khu hoi:", res["command_rtt_ms"])
if "telemetry_one_way_ms" in res: print("  telemetry mot chieu:", res["telemetry_one_way_ms"])
print("  tre bao SENSOR_TIMEOUT (s):", det)
print(f"\nDa ghi {os.path.normpath(OUT)}")
