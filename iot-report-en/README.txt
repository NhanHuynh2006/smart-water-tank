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

STRUCTURE (8 chapters)
  1 Introduction
  2 Theoretical background and system requirements
  3 Architecture and hardware design
  4 Protocol and communication
  5 Backend, data management and interface
  6 Control and system intelligence
  7 Security and reliability
  8 Experiments, results, limitations, conclusion, member contributions
    (8.4 latency and outage, 8.5 Wi-Fi power saving, both measured on hardware)

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
1. main.tex: names and IDs of all 5 members (\cstuname).
2. chapter08.tex "Member contributions": the 5 row table.
3. Table 8.2.1 (E1 level calibration): run  bench.py level  (IOT/docs/TESTING.md).
4. Table 8.4.1 (E5): OVERFLOW, no_flow, NO_CURRENT, manual rejection rows:
   run  bench.py fault  for each.
5. E6 outage rows in Table 8.5.1: rerun  bench.py outage  on the final firmware.
6. Replace the dashboard placeholder (fig/dashboard.png) with a live screenshot.

NOTES
- No en dash or em dash anywhere in the text.
