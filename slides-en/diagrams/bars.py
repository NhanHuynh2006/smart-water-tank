MONO = "font-family:'IBM Plex Mono', 'Courier New', monospace"
def colbar(x, y0, y1, w, color):
    # rounded data end (4px), square at baseline
    top, bot = min(y0, y1), max(y0, y1); h = bot - top
    r = min(4, h)
    if y1 < y0:  # grows up: round top
        d = f"M{x:.1f} {bot:.1f} V{top+r:.1f} Q{x:.1f} {top:.1f} {x+r:.1f} {top:.1f} H{x+w-r:.1f} Q{x+w:.1f} {top:.1f} {x+w:.1f} {top+r:.1f} V{bot:.1f} Z"
    else:        # grows down: round bottom
        d = f"M{x:.1f} {top:.1f} V{bot-r:.1f} Q{x:.1f} {bot:.1f} {x+r:.1f} {bot:.1f} H{x+w-r:.1f} Q{x+w:.1f} {bot:.1f} {x+w:.1f} {bot-r:.1f} V{top:.1f} Z"
    return f'<path d="{d}" fill="{color}"/>'
def p(x, y, w, txt, align='center', color='#646b76', mono=True, size=24, weight=None):
    st = f"position:absolute; left:{x:.0f}px; top:{y:.0f}px; width:{w:.0f}px; font-size:{size}px; text-align:{align}; color:{color}"
    if mono: st += "; " + MONO
    if weight: st += f"; font-weight:{weight}"
    return f'<p style="{st}">{txt}</p>'

# ---------- E2 volume error per cycle ----------
def e2():
    W, H = 900, 560; L, R, T, B = 80, 20, 40, 64
    cyc = [("16:56", -15.2), ("17:09", -12.6), ("17:17", -0.3), ("17:32", 8.2), ("17:55", -14.1), ("18:02", 3.0)]
    lo, hi = -20, 10
    pw, ph = W-L-R, H-T-B
    Y = lambda v: T + (hi - v) / (hi - lo) * ph
    band = pw / len(cyc); bw = 56
    svg = [f'<svg aria-label="Volume error per fill cycle: minus 15.2, minus 12.6, minus 0.3, plus 8.2, minus 14.1 and plus 3.0 percent; mean minus 5.2 percent" style="position:absolute; left:0px; top:0px" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    for v in (-20, -10, 10):
        svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(v):.1f}" y2="{Y(v):.1f}" stroke="#e1e0d9" stroke-width="1"/>')
    labels = []
    for i, (t, v) in enumerate(cyc):
        x = L + band*i + (band-bw)/2
        svg.append(colbar(x, Y(0), Y(v), bw, '#2a78d6'))
        ty = Y(v) - 40 if v >= 0 else Y(v) + 6
        labels.append(p(x-30, ty, bw+60, f"{v:+.1f}", color='#0e1a2b'))
        labels.append(p(x-30, H-B+14, bw+60, t))
    svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(0):.1f}" y2="{Y(0):.1f}" stroke="#898781" stroke-width="2"/>')
    svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(-5.2):.1f}" y2="{Y(-5.2):.1f}" stroke="#eb6834" stroke-width="2" stroke-dasharray="8 6"/>')
    svg.append('</svg>')
    for v in (-20, -10, 0, 10):
        labels.append(p(0, Y(v)-17, L-14, f"{v:+d}" if v else "0", align='right'))
    labels.append(p(L+band*2+8, Y(-5.2)+6, 200, "mean −5.2 %", align='left', color='#b4461a'))
    return '\n'.join(svg), '\n'.join(labels), (W, H)

# ---------- E7 latency median / p95 ----------
def e7():
    W, H = 1000, 560; L, R, T, B = 80, 20, 40, 80
    st = [("First", 226, 575), ("Sleep on", 462, 884), ("Sleep off", 187, 315), ("Final", 42, 138)]
    hi = 1000; pw, ph = W-L-R, H-T-B
    Y = lambda v: T + (1 - v/hi) * ph
    band = pw/len(st); bw = 40; gap = 2
    svg = [f'<svg aria-label="Command round trip median and 95th percentile by firmware stage: first 226 and 575 ms, modem sleep on 462 and 884, sleep off 187 and 315, final 42 and 138" style="position:absolute; left:0px; top:0px" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    for v in (250, 500, 750, 1000):
        svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(v):.1f}" y2="{Y(v):.1f}" stroke="#e1e0d9" stroke-width="1"/>')
    labels = []
    for i, (n, m, q) in enumerate(st):
        cx = L + band*i + band/2
        x1 = cx - bw - gap/2; x2 = cx + gap/2
        svg.append(colbar(x1, Y(0), Y(m), bw, '#2a78d6'))
        svg.append(colbar(x2, Y(0), Y(q), bw, '#eb6834'))
        labels.append(p(x1-14, Y(m)-38, bw+28, str(m), color='#0e1a2b'))
        labels.append(p(x2-14, Y(q)-38, bw+28, str(q), color='#0e1a2b'))
        labels.append(p(cx-110, H-B+14, 220, n, mono=False, color='#3d4552'))
    svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(0):.1f}" y2="{Y(0):.1f}" stroke="#c3c2b7" stroke-width="1"/>')
    svg.append('</svg>')
    for v in (0, 500, 1000):
        labels.append(p(0, Y(v)-17, L-14, str(v), align='right'))
    return '\n'.join(svg), '\n'.join(labels), (W, H)

# ---------- daily volume ----------
def daily():
    W, H = 760, 460; L, R, T, B = 64, 16, 40, 64
    days = [("21 Sep", 16.0, 26.0), ("22 Sep", 9.2, 18.8), ("23 Sep", 21.2, 18.2)]
    hi = 30; pw, ph = W-L-R, H-T-B
    Y = lambda v: T + (1 - v/hi) * ph
    band = pw/len(days); bw = 48; gap = 2
    svg = [f'<svg aria-label="Litres pumped in and drawn out per day: 21 September 16.0 in and 26.0 out, 22 September 9.2 and 18.8, 23 September 21.2 and 18.2" style="position:absolute; left:0px; top:0px" width="{W}" height="{H}" viewBox="0 0 {W} {H}">']
    for v in (10, 20, 30):
        svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(v):.1f}" y2="{Y(v):.1f}" stroke="#e1e0d9" stroke-width="1"/>')
    labels = []
    for i, (n, a, b) in enumerate(days):
        cx = L + band*i + band/2; x1 = cx - bw - gap/2; x2 = cx + gap/2
        svg.append(colbar(x1, Y(0), Y(a), bw, '#2a78d6')); svg.append(colbar(x2, Y(0), Y(b), bw, '#eb6834'))
        labels.append(p(x1-8, Y(a)-38, bw+16, f"{a:.1f}", color='#0e1a2b'))
        labels.append(p(x2-8, Y(b)-38, bw+16, f"{b:.1f}", color='#0e1a2b'))
        labels.append(p(cx-100, H-B+14, 200, n, mono=False, color='#3d4552'))
    svg.append(f'<line x1="{L}" x2="{W-R}" y1="{Y(0):.1f}" y2="{Y(0):.1f}" stroke="#c3c2b7" stroke-width="1"/>')
    svg.append('</svg>')
    for v in (0, 10, 20, 30):
        labels.append(p(0, Y(v)-17, L-12, str(v), align='right'))
    return '\n'.join(svg), '\n'.join(labels), (W, H)

for name, f in (("e2", e2), ("e7", e7), ("daily", daily)):
    s, l, wh = f()
    open(f"{name}.svg.html", "w").write(s); open(f"{name}.labels.html", "w").write(l)
    print(name, wh, len(s), l.count('<p'))
