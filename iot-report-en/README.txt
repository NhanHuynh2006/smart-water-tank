IOT FINAL PROJECT REPORT - SMART WATER TANK (ENGLISH VERSION)
=============================================================
Body (Chapter 1 to the end of member contributions): 33 pages. Full PDF 44 pages.
Group of 5 members.

BUILD
  pdflatex main.tex
  bibtex   main
  pdflatex main.tex
  pdflatex main.tex
On Overleaf: create a new project, upload this whole folder, Compiler = pdfLaTeX.

STRUCTURE (5 chapters + 1 appendix)
  1 Introduction and background   (motivation, scope, requirements, theory)
  2 System architecture, hardware and protocol
  3 Control and system intelligence
  4 Backend, interface, security and reliability
  5 Experiments, results and conclusion (limitations, member contributions)
  A Bench fault log
  Cross references use \label/\ref, so section numbers update themselves.

STATUS (27/09)
- Chapters 2 to 8 rewritten to match the final system: requirement table
  follows the Project 08 brief, flow derived from level, IQR + alpha-beta
  filter, parallel model, pump health classifier, password sessions and
  public tunnel, daily volume, 11 fault rules, new E2/E3/E4/E7 results.
- Figures generated from the database: python3 IOT/tools/make_figures.py
  (fig/e4_level_cycles.png, fig/e7_rtt_hist.png, fig/daily_volume.png).
  fig/architecture.png rendered from Mermaid.
- Numbers come from IOT/tools/analyze_experiments.py -> IOT/docs/results.json.

STILL TO FILL
1. DONE: member names and IDs on the cover and in the contribution table.
2. chapter05.tex "Member contributions": the 5 row table.
3. Table 5.2.1 (E1 level calibration): run  bench.py level  (IOT/docs/TESTING.md).
4. Table 5.4.1 (E5): OVERFLOW, no_flow, NO_CURRENT, manual rejection rows:
   run  bench.py fault  for each.
5. E6 outage rows in Table 5.5.1: rerun  bench.py outage  on the final firmware.
6. Replace the dashboard placeholder (fig/dashboard.png) with a live screenshot.

NOTES
- No en dash or em dash anywhere in the text.
