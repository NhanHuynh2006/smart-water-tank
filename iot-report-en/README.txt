IOT FINAL PROJECT REPORT - SMART WATER TANK (ENGLISH VERSION)
=============================================================
Body (Chapter 1 to the end of member contributions): 33 pages. Full PDF 63 pages.
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

STATUS (05/10)
- Numbers from the 5 October hardware session are in: E1 stepwise calibration
  (12 points, blind zone), E6 outage on the final firmware with the pump
  running, E7 round trip n = 40, automated system test T1 to T5.
  Raw data and summary: IOT/docs/SO_LIEU_20261005.md.
- New figures: fig/power.tex and fig/wiring.tex (TikZ), fig/e1_calibration.png,
  fig/e6_outage.png, fig/e7_rtt_final.png (python3 IOT/tools/make_figures.py).
- Stated as open on purpose: flow sensors partly validated (inlet calibrated,
  not used for accounting), NO_CURRENT not injected, MQTT not encrypted.

STILL TO DO
1. Photos of the prototype and the setup (see below).
2. Optional: pull one pump wire during the demo to time NO_CURRENT.

NOTES
- No en dash or em dash anywhere in the text.

Anh that (tu dong chen khi co file):
  fig/prototype.jpg  anh mo hinh da lap rap
  fig/setup.jpg      anh bo tri thi nghiem (vong nuoc, van xa, laptop)
Chep anh vao dung ten roi bien dich lai; khong co file thi hinh tu bo qua.
