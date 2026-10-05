#!/usr/bin/env python3
"""Ve hinh cho bao cao tu co so du lieu that.

    python3 IOT/tools/make_figures.py        (python he thong, can matplotlib)

Ghi PNG vao iot-report-en/fig/. Cung cua so thoi gian voi analyze_experiments.py.
"""
import os, sqlite3, time
from datetime import datetime

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

HERE = os.path.dirname(os.path.abspath(__file__))
DB = os.path.join(HERE, "..", "backend", "watertank.db")
FIG = os.path.join(HERE, "..", "..", "iot-report-en", "fig")
T0 = time.mktime(time.strptime("2026-09-23 16:48", "%Y-%m-%d %H:%M"))
T1 = time.mktime(time.strptime("2026-09-23 18:12", "%Y-%m-%d %H:%M"))

c = sqlite3.connect(DB)
plt.rcParams.update({"font.size": 10, "axes.grid": True, "grid.alpha": 0.3,
                     "figure.dpi": 150, "savefig.bbox": "tight"})
BLUE, ORANGE, GREY = "#3987e5", "#d95926", "#666666"


def level_cycles():
    rows = c.execute("SELECT recv_ts, level_pct, pump, level_ok, mode FROM telemetry "
                     "WHERE recv_ts BETWEEN ? AND ? AND replay = 0 ORDER BY recv_ts",
                     (T0 + 8 * 60, T1)).fetchall()   # bat dau tu chu ky tu dong dau tien
    t = [datetime.fromtimestamp(r[0]) for r in rows]
    lv = [r[1] if r[3] else float("nan") for r in rows]
    fig, ax = plt.subplots(figsize=(8, 3.2))
    # to bong cac doan bom chay
    start = None
    for ti, r in zip(t, rows):
        if r[2] and start is None:
            start = ti
        elif not r[2] and start is not None:
            ax.axvspan(start, ti, color=BLUE, alpha=0.12, lw=0)
            start = None
    # che do tay (thu lenh, tat bom de xa can) to xam, de nguoi doc khong
    # tuong bo dieu khien tu dong de muc tut duoi 30 %
    start = None
    for ti, r in zip(t, rows):
        if r[4] == "MANUAL" and start is None:
            start = ti
        elif r[4] != "MANUAL" and start is not None:
            ax.axvspan(start, ti, color=GREY, alpha=0.15, lw=0, hatch="///", fill=False)
            start = None
    ax.plot(t, lv, color=BLUE, lw=1.1, label="level (%)")
    for y, lab, col in ((30, "low 30 %", GREY), (70, "high 70 %", GREY), (85, "overflow 85 %", ORANGE)):
        ax.axhline(y, ls="--", lw=0.8, color=col)
        ax.text(t[-1], y + 1, lab, ha="right", va="bottom", fontsize=8, color=col)
    ax.set_ylim(0, 100)
    ax.set_ylabel("level (% of 14 cm)")
    ax.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
    ax.set_title("Bench run 23 Sep: blue = pump on, hatched = manual mode")
    fig.savefig(os.path.join(FIG, "e4_level_cycles.png"))
    plt.close(fig)


def rtt_hist():
    v = [r[0] for r in c.execute("SELECT latency_ms FROM commands WHERE latency_ms IS NOT NULL "
                                 "AND sent_ts BETWEEN ? AND ?", (T0, T1))]
    fig, ax = plt.subplots(figsize=(5, 2.8))
    ax.hist(v, bins=20, color=BLUE, edgecolor="white")
    v.sort()
    med = v[len(v) // 2]
    ax.axvline(med, color=ORANGE, lw=1.2)
    ax.text(med, ax.get_ylim()[1] * 0.9, f" median {med:.0f} ms", color=ORANGE, fontsize=8)
    ax.set_xlabel("command round trip (ms)")
    ax.set_ylabel("commands")
    ax.set_title(f"E7 command round trip, n = {len(v)}")
    fig.savefig(os.path.join(FIG, "e7_rtt_hist.png"))
    plt.close(fig)


def daily():
    q = """SELECT day,
       SUM(CASE WHEN pump_prev = 1 AND di > 0 AND di < 2 THEN di ELSE 0 END),
       SUM(CASE WHEN do_ > 0 AND do_ < 2 THEN do_ ELSE 0 END)
     FROM (SELECT date(CASE WHEN ts > 1600000000 THEN ts ELSE recv_ts END,'unixepoch','localtime') AS day,
                  LAG(pump) OVER w AS pump_prev,
                  volume_l - LAG(volume_l) OVER w AS di,
                  volume_out_l - LAG(volume_out_l) OVER w AS do_
           FROM telemetry WINDOW w AS (ORDER BY recv_ts))
     GROUP BY day HAVING day >= '2026-09-21' ORDER BY day"""
    rows = c.execute(q).fetchall()
    days = [r[0][5:] for r in rows]
    x = range(len(rows))
    fig, ax = plt.subplots(figsize=(5, 2.8))
    ax.bar([i - 0.2 for i in x], [r[1] for r in rows], 0.4, color=BLUE, label="pumped in")
    ax.bar([i + 0.2 for i in x], [r[2] or 0 for r in rows], 0.4, color=ORANGE, label="drawn out")
    ax.set_xticks(list(x), days)
    ax.set_ylabel("litres")
    ax.legend(fontsize=8)
    ax.set_title("Daily volume from /api/volume/daily")
    fig.savefig(os.path.join(FIG, "daily_volume.png"))
    plt.close(fig)


# ---- 05/10: firmware cuoi, hinh hoc 15,88 cm ----
DOCS = os.path.join(HERE, "..", "docs")
K_OLD, K_NEW, RULER_CM = 98.0, 85.5, 4.9      # hai luot step17 chay voi K = 98


def stepcal(name):
    """(the tich bom tich luy L theo K moi, phan vi 25 khoang cach) cho moi bac."""
    pings, vol = {}, {}
    for line in open(os.path.join(DOCS, name)):
        p = line.strip().split(",")
        if p[0] == "P" and float(p[2]) > 0:
            pings.setdefault(int(p[1]), []).append(float(p[2]))
        elif p[0] == "S":
            vol[int(p[1])] = float(p[2]) * K_OLD / K_NEW
    out = []
    for k in sorted(vol):
        v = sorted(pings[k])
        out.append((vol[k], v[len(v) // 4]))
    return out


def calibration_points():
    """Muc chuan (thuoc + the tich bom / day 100 cm2) va muc cam bien, cm."""
    lo, hi = stepcal("stepcal_20261005.txt"), stepcal("stepcal_high_20261005.txt")
    pts = []
    v_end = lo[-1][0]                 # luot thap ket thuc dung luc doc thuoc 4,9 cm
    for v, d in lo[1:]:               # bac 0 la day kho, khong co mat nuoc
        pts.append(("low", RULER_CM - (v_end - v) * 10, 15.88 - d))
    for v, d in hi[1:]:
        pts.append(("high", RULER_CM + v * 10, 15.88 - d))
    return pts


def e1_calibration():
    pts = calibration_points()
    blind = [p for p in pts if 6.9 < p[1] < 8.2]
    good = [p for p in pts if p not in blind]
    fig, ax = plt.subplots(figsize=(5.6, 3.4))
    ax.axvspan(7.2, 8.0, color=GREY, alpha=0.15, lw=0)
    ax.text(7.05, -5.4, "blind zone: water surface\n7.9 to 8.7 cm below the sensor", ha="right",
            va="center", fontsize=7.5, color="#333333")
    ax.plot([0, 12.4], [0, 12.4], color=GREY, lw=0.8, ls="--", label="reading = reference")
    ax.scatter([p[1] for p in good], [p[2] for p in good], s=28, color=BLUE, zorder=3,
               edgecolor="white", lw=0.8, label="outside the blind zone")
    ax.scatter([p[1] for p in blind], [p[2] for p in blind], s=34, marker="X", color=ORANGE,
               zorder=3, edgecolor="white", lw=0.6, label="stray echo, rejected by the gates")
    ax.set_xlim(0, 12.4); ax.set_ylim(-7, 12.4)
    ax.set_xlabel("reference level (cm)")
    ax.set_ylabel("ultrasonic level (cm)")
    ax.legend(fontsize=7.5, loc="upper left")
    fig.savefig(os.path.join(FIG, "e1_calibration.png"))
    plt.close(fig)
    return pts


def e6_outage():
    cut = time.mktime(time.strptime("2026-10-05 20:32:31", "%Y-%m-%d %H:%M:%S")) + 0.918
    back = cut + 120.045
    rows = c.execute("SELECT ts, level_pct, pump, replay, level_ok FROM telemetry WHERE ts BETWEEN ? AND ? "
                     "ORDER BY seq", (cut - 40, back + 110)).fetchall()
    dt_ = lambda x: datetime.fromtimestamp(x)
    groups = (("received live", BLUE, [r for r in rows if not r[3] and r[4]]),
              ("received live, readings rejected (last value held)", GREY, [r for r in rows if not r[3] and not r[4]]),
              ("buffered on the device, replayed after reconnection", ORANGE, [r for r in rows if r[3]]))
    stop = next(r for r in rows if r[0] > back and not r[2])
    peak = max(r[1] for r in rows if r[0] > stop[0])
    fig, ax = plt.subplots(figsize=(8, 3.4))
    ax.axvspan(dt_(cut), dt_(back), color=GREY, alpha=0.13, lw=0)
    for lab, col, g in groups:
        ax.plot([dt_(r[0]) for r in g], [r[1] for r in g], ".", ms=3, color=col, label=lab)
    x0 = dt_(cut - 38)
    for y, lab in ((70, "high 70 %"), (30, "low 30 %")):
        ax.axhline(y, ls="--", lw=0.8, color=GREY)
        ax.text(x0, y + 1, lab, ha="left", va="bottom", fontsize=8, color="#333333")
    ax.text(dt_(cut + 60), 90, "device traffic dropped for 120 s\nwhile the pump was running",
            ha="center", va="center", fontsize=8, color="#333333")
    ax.annotate(f"pump stopped by the device at {stop[1]:.1f} %,\npeak {peak:.1f} %",
                (dt_(stop[0]), stop[1]), xytext=(dt_(stop[0] - 12), 90), fontsize=8, color="#333333",
                ha="center", va="center", arrowprops=dict(arrowstyle="-", color="#333333", lw=0.8))
    ax.set_ylim(0, 100)
    ax.set_ylabel("level (% of 12.4 cm)")
    ax.xaxis.set_major_locator(mdates.SecondLocator(bysecond=[0]))
    ax.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
    ax.legend(fontsize=7.5, loc="lower right")
    fig.savefig(os.path.join(FIG, "e6_outage.png"))
    plt.close(fig)
    return stop[1], peak


def rtt_hist_final():
    a = time.mktime(time.strptime("2026-10-05 20:36:00", "%Y-%m-%d %H:%M:%S"))
    v = sorted(r[0] for r in c.execute("SELECT latency_ms FROM commands WHERE latency_ms IS NOT NULL "
                                       "AND action = 'mode' AND sent_ts BETWEEN ? AND ?", (a, a + 55)))
    fig, ax = plt.subplots(figsize=(5, 2.8))
    ax.hist(v, bins=16, color=BLUE, edgecolor="white")
    med = v[len(v) // 2] if len(v) % 2 else (v[len(v) // 2 - 1] + v[len(v) // 2]) / 2
    ax.axvline(med, color=ORANGE, lw=1.2)
    ax.text(med, ax.get_ylim()[1] * 0.9, f" median {med:.1f} ms", color="#333333", fontsize=8)
    ax.set_xlabel("command round trip (ms)")
    ax.set_ylabel("commands")
    ax.set_title(f"E7 command round trip, final firmware, n = {len(v)}")
    fig.savefig(os.path.join(FIG, "e7_rtt_final.png"))
    plt.close(fig)
    return v


if __name__ == "__main__":
    os.makedirs(FIG, exist_ok=True)
    level_cycles(); rtt_hist(); daily()
    for p in e1_calibration():
        print("E1 %-4s ref %5.2f  doc %5.2f  sai %+5.2f" % (p[0], p[1], p[2], p[2] - p[1]))
    print("E6 dung, dinh:", e6_outage())
    v = rtt_hist_final(); print("E7 cuoi: n", len(v), "min", v[0], "max", v[-1])
    print("da ve:", sorted(f for f in os.listdir(FIG) if f.endswith(".png")))
