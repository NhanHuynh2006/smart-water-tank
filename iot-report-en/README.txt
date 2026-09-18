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

ALREADY MEASURED ON REAL HARDWARE (filled in, do not overwrite)
- E7 command round trip: median 225.8 ms, p95 575.3 ms, n=14. This is the
  result of record. Measured with one server clock at both ends.
- E7 telemetry one way: 245.0 ms median at millisecond resolution, reported
  for comparison only. The old second resolution field gave 844.6 ms, which
  was a quantisation artefact and is shown alongside to document the fix.
- E7 outage: 60.0 s, 32 messages buffered and replayed (seq 598 to 629),
  10 lost inside the keep alive detection window (seq 588 to 597),
  buffer capacity 240, pump stayed off throughout.
- E9 Wi-Fi power saving, new Section 8.5: median 462 -> 187 ms, p95 884 ->
  315 ms after adding WiFi.setSleep(false). n=12 each side.
- Limitations now also record the chart pagination defect: the history query
  limited after ordering ascending, so it returned the OLDEST rows of the
  window. Invisible at the specified 1 Hz (600 rows against a 2000 row limit),
  it surfaced only at 10x simulator speed (about 3000 rows), where the charts
  froze in the past while the live panel kept updating.

STILL TO FILL
1. main.tex: fill in the names and IDs of all 5 members (\cstuname).
2. chapter08.tex Section 8.7: fill the 5 row contribution table.
3. E1 and E2 sensor calibration, and E3 to E6, need the physical tank.
   E8 currently carries the simulated sweep; repeat on hardware when built.
4. Replace the 2 \chohinh{...}{...} placeholder boxes with real figures.
5. Delete the "Discussion points" notes once the real discussion is written.

NOTES
- No en dash or em dash anywhere in the text.
- Originality checked against the extracted text of all six lecture slide decks:
  zero matching sequences of 5 or more consecutive words.
