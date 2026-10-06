# Presentation script — Smart Water Tank (Project 08)

Short version: only the key point of each slide, about 7 minutes, then the live demo. The same lines are the speaker notes in the deck: https://claude.ai/artifact/TmihFwubP3Ltox1HqgkGtL

| # | Slide | Presenter | Time | Ends at |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:15 | 0:15 |
| 2 | Seven parts, then the live demo | Bảo | 0:05 | 0:20 |
| 3 | 01 · Problem and requirements | Bảo | 0:05 | 0:25 |
| 4 | What a smart tank must do | Bảo | 0:10 | 0:35 |
| 5 | Requirements and where they are met | Bảo | 0:15 | 0:50 |
| 6 | 02 · Architecture and hardware | Bảo | 0:05 | 0:55 |
| 7 | System architecture | Bảo | 0:20 | 1:15 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 0:15 | 1:30 |
| 9 | The bench rig | Bảo | 0:15 | 1:45 |
| 10 | The prototype on the bench | Bảo | 0:15 | 2:00 |
| 11 | Controller board schematic | Bảo | 0:15 | 2:15 |
| 12 | Three signal lessons, each found by measuring | Bảo | 0:15 | 2:30 |
| 13 | 03 · MQTT protocol | Nhân | 0:05 | 2:35 |
| 14 | MQTT topic design | Nhân | 0:15 | 2:50 |
| 15 | Commands with acknowledgement | Nhân | 0:10 | 3:00 |
| 16 | 04 · Control and intelligence | Nhân | 0:05 | 3:05 |
| 17 | Level measurement pipeline | Nhân | 0:10 | 3:15 |
| 18 | Model bridging and flow from level | Nhân | 0:10 | 3:25 |
| 19 | Controller state machine | Nhân | 0:10 | 3:35 |
| 20 | Eleven fault rules | Nhân | 0:10 | 3:45 |
| 21 | Safety guard and overflow layers | Nhân | 0:10 | 3:55 |
| 22 | Pump health classification | Nhân | 0:10 | 4:05 |
| 23 | Rolling baseline and daily volume | Nhân | 0:15 | 4:20 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:05 | 4:25 |
| 25 | Backend and data model | Ngân | 0:05 | 4:30 |
| 26 | Dashboard and remote access | Ngân | 0:10 | 4:40 |
| 27 | Security model | Ngân | 0:10 | 4:50 |
| 28 | Behaviour during a network outage | Ngân | 0:10 | 5:00 |
| 29 | 06 · Experiments and results | Uyên | 0:05 | 5:05 |
| 30 | Experiment plan | Uyên | 0:10 | 5:15 |
| 31 | Level calibration against a ruler | Uyên | 0:15 | 5:30 |
| 32 | Control response and switching | Uyên | 0:10 | 5:40 |
| 33 | Volume estimation error | Uyên | 0:10 | 5:50 |
| 34 | Command latency | Uyên | 0:05 | 5:55 |
| 35 | Fault detection | Uyên | 0:10 | 6:05 |
| 36 | Network outage with the pump running | Uyên | 0:10 | 6:15 |
| 37 | Automated system test, 5/5 pass | Uyên | 0:05 | 6:20 |
| 38 | 07 · Lessons and conclusion | Dương | 0:05 | 6:25 |
| 39 | Bugs found on the bench | Dương | 0:10 | 6:35 |
| 40 | Limitations and next steps | Dương | 0:10 | 6:45 |
| 41 | Conclusion | Dương | 0:10 | 6:55 |
| 42 | Live demonstration | Dương | 0:15 | 7:10 |
| 43 | Thank you | Dương | 0:05 | 7:15 |

**1. Smart Water Tank** (Bảo)  
Good morning. We are group [number], Project 08, the Smart Water Tank. One idea to remember: every control and safety decision runs on the ESP32, so the tank keeps working without the network.

**2. Seven parts, then the live demo** (Bảo)  
Seven parts, then a live demo on the real tank.

**3. 01 · Problem and requirements** (Bảo)  
Part one: the problem.

**4. What a smart tank must do** (Bảo)  
The brief asks for six things: measure, control, protect, diagnose, supervise, and keep working offline. The last one shaped the whole design.

**5. Requirements and where they are met** (Bảo)  
Every requirement is done except two, stated honestly: the flow sensors are fitted and the inlet is calibrated, but control still uses flow from the level; and the leak baseline needs more days of data.

**6. 02 · Architecture and hardware** (Bảo)  
Part two: architecture and hardware.

**7. System architecture** (Bảo)  
Sensors feed the ESP32, which closes the loop every 200 ms. MQTT carries telemetry to the broker and commands back. FastAPI stores data in SQLite and serves the dashboard; a tunnel makes it public. The only path to the pump goes through the ESP32.

**8. The server never switches the pump. It asks, and the ESP32 decides.** (Bảo)  
Our key decision: anything that needs a safe stop runs on the ESP32. The server only keeps history and slow analysis. Lose the network and you lose the view, not the control.

**9. The bench rig** (Bảo)  
A small tank, 10 by 10 cm, 1.24 litres. The pump gives 0.36 litres per minute, five times less than the datasheet, and we size every threshold from that measured number.

**10. The prototype on the bench** (Bảo)  
This is the real rig: the tank with the sensor and float in the lid, the water loop with two flow sensors and a valve, and the controller board on the source bucket.

**11. Controller board schematic** (Bảo)  
Our board. 12 V comes in and two modules give 5 V and 3.3 V. Every 5 V sensor signal goes through a 10k/20k divider; the relay, floats and reset button sit on the 3.3 V rail.

**12. Three signal lessons, each found by measuring** (Bảo)  
Three wiring lessons, all found by measuring: a level shifter that ate the echo, a floating input that read mains, and pump noise that disappeared only after we rebuilt the power stage.

**13. 03 · MQTT protocol** (Nhân)  
Part three: the MQTT protocol.

**14. MQTT topic design** (Nhân)  
One topic per job, with QoS chosen by the cost of losing a message. Status uses the last will, every message has a sequence number, and the device account cannot publish commands.

**15. Commands with acknowledgement** (Nhân)  
A button press becomes a command with an id. The screen changes only when the ESP32 acknowledges it, with the reason if it refuses.

**16. 04 · Control and intelligence** (Nhân)  
Part four: control and intelligence.

**17. Level measurement pipeline** (Nhân)  
One ping is never trusted. Each reading passes five steps, from a 15-ping window to an alpha-beta filter. In parallel, a raw-distance check can trip overflow on its own.

**18. Model bridging and flow from level** (Nhân)  
A tank model runs beside the sensor. When the sensor is blind, the model carries control for up to 90 seconds; then the pump stops.

**19. Controller state machine** (Nhân)  
Seven states with a 30 to 70 percent band and minimum on and off times. Fault states are sticky, so a faulty pump never restarts by itself.

**20. Eleven fault rules** (Nhân)  
Eleven fault rules, checked every 200 ms. Any rule stops the pump. NO_PROGRESS is the brief's 'pump on but no flow' rule, measured by the level.

**21. Safety guard and overflow layers** (Nhân)  
Every pump start, automatic or manual, goes through one guard function. Overflow has seven independent layers, from the 70 percent stop to the mechanical float.

**22. Pump health classification** (Nhân)  
Current plus water movement tells two faults apart: current but no water is a hydraulic problem; no current is an electrical one.

**23. Rolling baseline and daily volume** (Nhân)  
The backend learns normal use for each half hour of the day and alarms on a sustained excess. It needs three days per slot, which we do not have yet.

**24. 05 · Backend, dashboard, security** (Ngân)  
Part five: backend, dashboard and security.

**25. Backend and data model** (Ngân)  
One Python process, six SQLite tables, a REST API. Commands and configuration need an admin session.

**26. Dashboard and remote access** (Ngân)  
Anyone with the link can watch; only an admin with the password can control. The tank drawing shows the thresholds at their real height.

**27. Security model** (Ngân)  
Broker passwords and ACLs, session cookies, login lockout. The float and minimum switching times stop attacks by design. Our open gap: MQTT in the lab is not encrypted.

**28. Behaviour during a network outage** (Ngân)  
When the network drops, the loop never waits, data goes into a 4-minute buffer, the broker announces the device offline, and the device reconnects by itself.

**29. 06 · Experiments and results** (Uyên)  
Part six: experiments and results.

**30. Experiment plan** (Uyên)  
Every result has an independent reference and comes from the database by a script. Two sessions: 23 September for control, 5 October for the final firmware.

**31. Level calibration against a ruler** (Uyên)  
Against a ruler, the level error is 0.68 cm outside one blind zone, where the sensor returns a stable false echo. The firmware rejects it and crosses the zone on the model.

**32. Control response and switching** (Uyên)  
Every automatic fill stopped between 70.1 and 70.8 percent, about 13 starts per hour, no chatter and no false alarm in 84 minutes.

**33. Volume estimation error** (Uyên)  
Volume error is minus 5.2 percent on average, because we assume a constant pump flow. The calibrated inlet sensor is the fix.

**34. Command latency** (Uyên)  
Commands are confirmed in 29 ms median. Turning off Wi-Fi power saving was the biggest gain.

**35. Fault detection** (Uyên)  
Faults stop the pump in the same 200 ms cycle. We did not inject NO_CURRENT; we measured its signal instead, 353 to 365 mA on and zero off.

**36. Network outage with the pump running** (Uyên)  
We cut the network for 120 seconds while pumping. The tank kept filling and stopped itself at 70.7 percent; 125 messages were replayed and only 4 lost.

**37. Automated system test, 5/5 pass** (Uyên)  
Our automated acceptance test runs without an operator: five tests, all pass.

**38. 07 · Lessons and conclusion** (Dương)  
Part seven: lessons and conclusion.

**39. Bugs found on the bench** (Dương)  
Four bugs, each found by counting: an inverted relay, a pump start after every reboot, a repeating false echo, and silently truncated messages.

**40. Limitations and next steps** (Dương)  
Open items: flow sensors only partly validated, current not checked with a meter, NO_CURRENT not injected, the blind zone, and no TLS on MQTT.

**41. Conclusion** (Dương)  
Control lives on the edge, safety is structural, and we trusted measurements over datasheets. Now the real tank.

**42. Live demonstration** (Dương)  
Eight steps on the real tank: automatic fill, a manual command, the float, a dry intake, network loss, a pulled pump wire, and a reboot. A backup video is ready.

**43. Thank you** (Dương)  
Thank you. We are happy to take questions.

## Appendix: likely questions and short answers

**Is the flow sensor requirement really met?** Not simply "met". Both YF-S401 sensors are fitted, pulled up, read by interrupt and published. Until 22 September the pump injected about 1500 Hz of conducted interference; after the 5 October rewiring the inlet sensor counts real water (0 Hz off, 20 to 30 Hz on) and we calibrated it against the ruler, K = 85.5. It now feeds the tank model. But control and the volume counter still use flow derived from the level, because the turbine is validated on one calibration run, and the outlet flow is below the sensor's 0.3 L/min start. So: implemented, partly validated, limitation stated.

**Was NO_CURRENT tested?** Not by physical injection; its 2 s is a design value. We measured the signal it depends on: 353 to 365 mA with the relay closed, 0 mA open, about eleven times the noise floor. We pull a pump wire in the demo.

**Is the system secured with TLS?** No. Inside the lab the broker runs on plain port 1883 with passwords and an ACL; the public tunnel carries only the dashboard over HTTPS. MQTTS on 8883 is the deployment step. We do not claim end-to-end TLS.

**The pump started by itself after a reboot once. Is that fixed?** Yes. The device stays in BOOT until a first reading is accepted. On 5 October it restarted at 33 %: the first level published was 32 %, never zero, and the pump stayed off. We repeat it live.

**Why did the outage test lose 4 messages?** The ring buffer only protects samples produced after the firmware decides the link is down. The final firmware closes a socket that refuses data for 3 s, so about 3 to 4 samples are lost; the first firmware waited for the 15 s keep alive and lost 10.

**Why a relay and not a MOSFET?** The pump has two states and switches a few times per hour, so the MOSFET's switching speed brings nothing; the relay module drives the load from its own supply. A 1N4007 flyback diode protects it.

**Why SQLite and not a time series database?** We write one row per second from one node, three orders of magnitude below where relational engines struggle. We still follow the time series schema rule: only the timestamp is indexed.

**Why polling and not WebSocket?** The device publishes at 1 Hz, so polling at 1 Hz wastes nothing, and a WebSocket adds a stateful channel with its own failure modes. Beyond about ten nodes or 5 Hz we would switch.

**What if the ESP32 itself hangs?** The relay is driven off as the first instruction after any reset, so a watchdog reset stops the pump. A hardware float cut-off in series with the pump would be the next step for a real installation.

**Why is the sensor timeout 90 seconds?** Crossing the ultrasonic blind zone takes up to 75 s during a fill, and the tank model bridges it; with 4 s every fill stopped early, and 45 s still stopped fills inside the blind zone. During the wait the float, the raw distance guard and the run limits still protect the tank.
