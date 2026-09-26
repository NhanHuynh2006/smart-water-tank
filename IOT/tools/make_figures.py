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


if __name__ == "__main__":
    os.makedirs(FIG, exist_ok=True)
    level_cycles(); rtt_hist(); daily()
    print("da ve:", sorted(f for f in os.listdir(FIG) if f.endswith(".png")))
