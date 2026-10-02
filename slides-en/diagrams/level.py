import sqlite3, time
db = sqlite3.connect('/home/nhanhuynh/Documents/IOT/IOT/backend/watertank.db')
T0 = time.mktime(time.strptime('2026-09-23 16:56:00', '%Y-%m-%d %H:%M:%S')); T1 = T0 + 76*60
rows = db.execute("SELECT recv_ts, level_pct, level_ok, pump, mode FROM telemetry WHERE recv_ts BETWEEN ? AND ? AND replay=0 ORDER BY recv_ts", (T0, T1)).fetchall()
W, H = 1664, 470
L, R, T, B = 72, 196, 16, 56
pw, ph = W - L - R, H - T - B
X = lambda t: L + (t - T0) / (T1 - T0) * pw
Y = lambda v: T + (1 - v / 100) * ph
# path, broken at gaps > 5 s or untrusted
d = []; last = None; acc = []; bucket = None
for t, v, ok, p, m in rows:
    if not ok or v is None: last = None; continue
    b = int((t - T0) // 5)
    if b == bucket: continue
    bucket = b
    cmd = 'M' if last is None or t - last > 8 else 'L'
    d.append(f"{cmd}{X(t):.1f} {Y(v):.1f}"); last = t
path = ''.join(d)
def spans(pred):
    out = []; s = None; prev = None
    for t, v, ok, p, m in rows:
        if pred(p, m) and s is None: s = t
        elif not pred(p, m) and s is not None: out.append((s, t)); s = None
        prev = t
    if s: out.append((s, prev))
    return out
pump = spans(lambda p, m: p)
man = spans(lambda p, m: m == 'MANUAL')
svg = [f'<svg aria-label="Tank level over 76 minutes: the level rises while the pump runs and every automatic fill stops near 70 percent, far below the 85 percent overflow line" style="position:absolute; left:0px; top:0px" width="{W}" height="{H}" viewBox="0 0 {W} {H}">',
       '<defs><pattern id="hatch" width="10" height="10" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><line x1="0" y1="0" x2="0" y2="10" stroke="#c3c2b7" stroke-width="2"/></pattern></defs>']
for a, b in man:
    svg.append(f'<rect x="{X(a):.1f}" y="{T}" width="{X(b)-X(a):.1f}" height="{ph}" fill="url(#hatch)" opacity="0.55"/>')
for a, b in pump:
    if X(b) - X(a) < 2: continue
    svg.append(f'<rect x="{X(a):.1f}" y="{T}" width="{X(b)-X(a):.1f}" height="{ph}" fill="#2a78d6" opacity="0.10"/>')
for v in (0, 25, 50, 75, 100):
    svg.append(f'<line x1="{L}" x2="{L+pw}" y1="{Y(v):.1f}" y2="{Y(v):.1f}" stroke="#e1e0d9" stroke-width="1"/>')
svg.append(f'<line x1="{L}" x2="{L+pw}" y1="{Y(0):.1f}" y2="{Y(0):.1f}" stroke="#c3c2b7" stroke-width="1"/>')
for v, col in ((30, '#898781'), (70, '#898781'), (85, '#eb6834')):
    svg.append(f'<line x1="{L}" x2="{L+pw}" y1="{Y(v):.1f}" y2="{Y(v):.1f}" stroke="{col}" stroke-width="2" stroke-dasharray="8 6"/>')
svg.append(f'<path d="{path}" fill="none" stroke="#2a78d6" stroke-width="3" stroke-linejoin="round" stroke-linecap="round"/>')
svg.append('</svg>')
s = '\n'.join(svg); print(len(s))
open('level.svg.html', 'w').write(s)
labels = []
for v in (0, 25, 50, 75, 100):
    labels.append(f'<p style="position:absolute; left:0px; top:{Y(v)-17:.0f}px; width:{L-14}px; font-size:24px; text-align:right; color:#646b76; font-family:\'IBM Plex Mono\', \'Courier New\', monospace">{v}</p>')
for hh, mm in ((17,0),(17,15),(17,30),(17,45),(18,0)):
    t = time.mktime(time.strptime(f'2026-09-23 {hh}:{mm:02d}:00', '%Y-%m-%d %H:%M:%S'))
    labels.append(f'<p style="position:absolute; left:{X(t)-60:.0f}px; top:{H-44}px; width:120px; font-size:24px; text-align:center; color:#646b76; font-family:\'IBM Plex Mono\', \'Courier New\', monospace">{hh}:{mm:02d}</p>')
for v, txt, col in ((85, 'overflow 85 %', '#b4461a'), (70, 'stop 70 %', '#3d4552'), (30, 'start 30 %', '#3d4552')):
    labels.append(f'<p style="position:absolute; left:{L+pw+16}px; top:{Y(v)-17:.0f}px; width:{R-16}px; font-size:24px; color:{col}">{txt}</p>')
open('level.labels.html', 'w').write('\n'.join(labels))
print(len(pump), len(man))
