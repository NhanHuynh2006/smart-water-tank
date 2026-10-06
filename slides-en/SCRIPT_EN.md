# Presentation script — Smart Water Tank (Project 08)

Each slide: the key point and why it matters, about 18 minutes in total, then the live demo. The same text is the speaker notes in the deck: https://claude.ai/artifact/TmihFwubP3Ltox1HqgkGtL

| # | Slide | Presenter | Time | Ends at |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:30 | 0:30 |
| 2 | Seven parts, then the live demo | Bảo | 0:15 | 0:45 |
| 3 | 01 · Problem and requirements | Bảo | 0:05 | 0:50 |
| 4 | What a smart tank must do | Bảo | 0:25 | 1:15 |
| 5 | Requirements and where they are met | Bảo | 0:30 | 1:45 |
| 6 | 02 · Architecture and hardware | Bảo | 0:05 | 1:50 |
| 7 | System architecture | Bảo | 0:35 | 2:25 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 0:35 | 3:00 |
| 9 | The bench rig | Bảo | 0:30 | 3:30 |
| 10 | The prototype on the bench | Bảo | 0:25 | 3:55 |
| 11 | Controller board schematic | Bảo | 0:35 | 4:30 |
| 12 | Three signal lessons, each found by measuring | Bảo | 0:35 | 5:05 |
| 13 | 03 · MQTT protocol | Nhân | 0:05 | 5:10 |
| 14 | MQTT topic design | Nhân | 0:30 | 5:40 |
| 15 | Commands with acknowledgement | Nhân | 0:30 | 6:10 |
| 16 | 04 · Control and intelligence | Nhân | 0:05 | 6:15 |
| 17 | Level measurement pipeline | Nhân | 0:35 | 6:50 |
| 18 | Model bridging and flow from level | Nhân | 0:35 | 7:25 |
| 19 | Controller state machine | Nhân | 0:35 | 8:00 |
| 20 | Eleven fault rules | Nhân | 0:30 | 8:30 |
| 21 | Safety guard and overflow layers | Nhân | 0:30 | 9:00 |
| 22 | Pump health classification | Nhân | 0:30 | 9:30 |
| 23 | Rolling baseline and daily volume | Nhân | 0:30 | 10:00 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:05 | 10:05 |
| 25 | Backend and data model | Ngân | 0:25 | 10:30 |
| 26 | Dashboard and remote access | Ngân | 0:30 | 11:00 |
| 27 | Security model | Ngân | 0:35 | 11:35 |
| 28 | Behaviour during a network outage | Ngân | 0:30 | 12:05 |
| 29 | 06 · Experiments and results | Uyên | 0:05 | 12:10 |
| 30 | Experiment plan | Uyên | 0:20 | 12:30 |
| 31 | Level calibration against a ruler | Uyên | 0:35 | 13:05 |
| 32 | Control response and switching | Uyên | 0:20 | 13:25 |
| 33 | Volume estimation error | Uyên | 0:30 | 13:55 |
| 34 | Command latency | Uyên | 0:25 | 14:20 |
| 35 | Fault detection | Uyên | 0:30 | 14:50 |
| 36 | Network outage with the pump running | Uyên | 0:30 | 15:20 |
| 37 | Automated system test, 5/5 pass | Uyên | 0:15 | 15:35 |
| 38 | 07 · Lessons and conclusion | Dương | 0:05 | 15:40 |
| 39 | Bugs found on the bench | Dương | 0:30 | 16:10 |
| 40 | Limitations and next steps | Dương | 0:25 | 16:35 |
| 41 | Conclusion | Dương | 0:25 | 17:00 |
| 42 | Live demonstration | Dương | 0:35 | 17:35 |
| 43 | Thank you | Dương | 0:05 | 17:40 |

**1. Smart Water Tank** (Bảo)  
Good morning. We are group [number], and our project is Project 08, the Smart Water Tank. We built a real small tank whose pump is controlled by an ESP32: it measures level and flow, detects faults, and can be watched from anywhere. If you remember one idea, remember this: every control and safety decision runs on the ESP32 itself, so the tank keeps working even when the network does not.

**2. Seven parts, then the live demo** (Bảo)  
The talk has seven parts: the problem, the architecture and hardware, the MQTT protocol, the control logic, the platform, our experiments, and our lessons. Then we switch to the real tank for a live demo.

**3. 01 · Problem and requirements** (Bảo)  
Part one: the problem and the requirements.

**4. What a smart tank must do** (Bảo)  
Most homes still use a mechanical float: cheap, but it only knows when the tank is full. The brief asks for six things: measure level and flow against a real reference, control the level without chattering, never overflow, diagnose pump faults, supervise remotely, and keep working when the network is gone. That last requirement, shown in the dark card, decided our whole architecture.

**5. Requirements and where they are met** (Bảo)  
This table is Table 1.3.1 of the report. Nearly every requirement is done and demonstrated. We are upfront about two. The flow sensors are fitted and the inlet sensor is calibrated, but control still uses flow derived from the level, so we call it partly validated. And the leak baseline is built, but it needs three days of data per half-hour slot before it may raise an alarm.

**6. 02 · Architecture and hardware** (Bảo)  
Part two: architecture and hardware.

**7. System architecture** (Bảo)  
The whole system is four blocks in series. Inside the blue boundary, the sensors feed the ESP32, which closes the control loop every 200 ms and switches the pump itself. The ESP32 talks MQTT to a Mosquitto broker; FastAPI stores everything in SQLite and serves the dashboard, and a Cloudflare tunnel makes it public over HTTPS. The key point is the dark box: the only path to the pump goes through the ESP32. The server can ask, but it never switches.

**8. The server never switches the pump. It asks, and the ESP32 decides.** (Bảo)  
This is our central design decision. The lectures say anything that needs an immediate safe stop must run at the edge, so we put filtering, the state machine, all fault rules and the safety guard on the ESP32. The server keeps only what needs days of memory: history, daily volume and the leak baseline. So when the network drops, we lose the view and remote commands, but the tank still fills at the right level and every safety rule still applies.

**9. The bench rig** (Bảo)  
The tank is deliberately small: a 10 by 10 cm base, so one centimetre is exactly 0.1 litre, and 12.4 cm of working range, 1.24 litres. The orange number matters most: the datasheet promises 1.67 litres per minute, but we measured 0.36, and only 0.24 with a low source bucket. Every timing threshold in the firmware is sized from these measured numbers, not from the datasheet.

**10. The prototype on the bench** (Bảo)  
This is the real rig. The main tank has the ultrasonic sensor and the upper float in its lid. Water goes in a loop: from the source bucket through the pump and the inlet flow sensor into the tank, and out through the drain valve and the outlet flow sensor. The controller board sits on the lid of the source bucket.

**11. Controller board schematic** (Bảo)  
This is our board, from the KiCad schematic. 12 V comes in and two step-down modules give a 5 V rail and a 3.3 V rail. Every 5 V sensor output, the echo, both flow sensors and the current sensor, goes through a 10k and 20k divider so the ESP32 sees at most 3.3 V. The relay module, the two floats and the reset button sit on the 3.3 V rail, and the relay uses GPIO 10, which needs the flash in DIO mode.

**12. Three signal lessons, each found by measuring** (Bảo)  
Three wiring lessons, each found by measuring, not by reading code. A level shifter distorted the echo and lost 30 percent of readings; a plain divider fixed it. A floating flow input picked up exactly 50 Hz from the mains with the valve closed. And the pump injected 1500 Hz of noise into the flow lines until we rebuilt the power stage on 5 October; since then the inlet counts real water and is calibrated, though control still uses flow from the level.

**13. 03 · MQTT protocol** (Nhân)  
Part three: the MQTT protocol.

**14. MQTT topic design** (Nhân)  
Each topic has one job, and we chose its QoS by asking what it costs to lose one message. Telemetry at 1 Hz uses QoS 0, because one lost sample changes nothing; faults, commands and acknowledgements use QoS 1. The status topic carries the last will, so the broker itself announces when the device disappears. Every message has a sequence number to count losses, and the ACL means the device account cannot publish commands.

**15. Commands with acknowledgement** (Nhân)  
This follows one press of the pump button. The dashboard sends a command with a unique id, the ESP32 checks its safety guard and answers on cmd/ack with the result, the reason and the real pump state. The button never changes on its own; the screen shows only what the device confirmed. If the device refuses, for example inside the 20 second rest time, the user sees the reason and a countdown.

**16. 04 · Control and intelligence** (Nhân)  
Part four: control and intelligence on the ESP32.

**17. Level measurement pipeline** (Nhân)  
In this narrow tank some echoes come back from the wall or the floor, so we never trust one ping. Each reading goes through five steps: a window of 15 pings, a spread check, the lower quartile, physical plausibility, and an alpha-beta filter that tracks level and rate without lag. Counting rejections showed one bad statistic was throwing away most readings; switching to the interquartile range raised acceptance from 60 to 93 percent. In parallel, a raw distance check can trip the overflow protection on its own.

**18. Model bridging and flow from level** (Nhân)  
Next to the sensor we run a simple model of the tank: inflow minus outflow over the base area, pulled towards every good reading. When the sensor goes blind, the model takes over control for at most 90 seconds, then the pump stops with LEVEL_LOST. This fixed fills that used to stop at about 50 percent. The model also gives the outflow on the dashboard, measured only when the pump is off so it does not jump when the pump starts.

**19. Controller state machine** (Nhân)  
The controller has seven states. It starts the pump below 30 percent and stops above 70, with a minimum on time of 3 s and off time of 20 s, so it never chatters. A manual start goes to MANUAL_ON only if the safety guard allows it. Any fault rule stops the pump and moves to a fault state, and those states are sticky: a pump fault waits for an operator, so a dry pump never restarts by itself.

**20. Eleven fault rules** (Nhân)  
The brief asks for one fault rule; we have eleven, checked every 200 ms, and any rule that fires stops the pump at once. OVERFLOW has three independent triggers: the float, the raw distance and the filtered level. NO_PROGRESS is the brief's example rule, pump on with no flow, measured by the level not rising. The sensor timeout is long on purpose, 90 seconds while the model is valid, because shorter limits stopped normal fills.

**21. Safety guard and overflow layers** (Nhân)  
Manual mode must not bypass safety, so every pump start, automatic or manual, goes through one single guard function. It is re-checked every cycle while a manual pump runs, so lifting the float stops it within 200 ms. Overflow is protected by seven independent layers, from the 70 percent stop to the mechanical float and the relay being switched off first at boot. No single failure disables them all.

**22. Pump health classification** (Nhân)  
This is advanced option A: classify pump faults from current and water movement. Current with water moving means a healthy pump. Current but no water after 25 seconds means a hydraulic problem: a dry intake or a blocked pipe. No current means an electrical problem: a broken wire, relay or motor. The same symptom, a tank that does not fill, now sends the technician to the right place.

**23. Rolling baseline and daily volume** (Nhân)  
This is advanced option B, running on the backend because it needs days of memory. The day is cut into 48 half-hour slots, each learning its normal use, and an alarm fires when use stays above mean plus three standard deviations for two slots in a row. Honestly, each slot needs three days of data before it may alarm, and we do not have that yet, so we show the mechanism, not a real detection.

**24. 05 · Backend, dashboard, security** (Ngân)  
Part five: backend, dashboard and security.

**25. Backend and data model** (Ngân)  
The backend is one Python process: it subscribes to MQTT, writes six SQLite tables, and serves a REST API. We chose SQLite because one row per second is far below where a relational database struggles. Reads use GET, configuration uses PUT, and commands use POST; anything that changes the physical world needs an admin session.

**26. Dashboard and remote access** (Ngân)  
The dashboard is one web page that works on a computer and on a phone; on a phone the panels stack into one column. Anyone with the public link can watch, but the controls appear only after logging in with the admin password. We first tried to allow control by IP address and dropped it, because through the tunnel every request looks like it comes from the laptop itself.

**27. Security model** (Ngân)  
Security is checked layer by layer. The broker needs a password and an ACL, the API needs a session cookie, and five wrong passwords lock the address. Two attacks are stopped by the control design itself: fake telemetry cannot cause overflow because the float is on the device, and spamming commands cannot wear the pump because of the minimum times. Our open gap is stated plainly: MQTT inside the lab is not encrypted; TLS on port 8883 is the deployment step.

**28. Behaviour during a network outage** (Ngân)  
When the network fails, four things keep the system correct. The control loop never waits for the network; we bound every network call and close a dead socket after 3 seconds. Telemetry goes into a 4-minute ring buffer and is replayed in order later. The broker publishes the last will so everyone knows the device is offline, and the device reconnects by itself and finds the broker again by name.

**29. 06 · Experiments and results** (Uyên)  
Part six: experiments and results.

**30. Experiment plan** (Uyên)  
Our rule: every result needs a reference independent of the system, and every number comes from the database by a script anyone can rerun. We analysed two sessions: 23 September for control quality, and 5 October on the final firmware for calibration, the outage test and the system test.

**31. Level calibration against a ruler** (Uyên)  
We calibrated the level with the valve closed, pumping in 20-second steps and using one ruler reading plus the pumped volume as the reference. Outside one band, the error is 0.68 cm RMS. Inside that band, when the water is about 8 cm below the sensor, all 40 pings return the same false echo. Because it is stable, it cannot be filtered as noise; the firmware rejects it and crosses the band on the tank model.

**32. Control response and switching** (Uyên)  
This is 76 minutes of real operation. Every automatic fill stopped between 70.1 and 70.8 percent, with about 1.4 percent overshoot from water still in the pipe. The pump started about 13 times per hour, the shortest rest was 32 seconds, so it never chattered, and no fault rule fired falsely.

**33. Volume estimation error** (Uyên)  
To check the volume, we compare the device's pumped volume with a reference from the level rise plus the measured drain. The mean error is minus 5.2 percent. The largest errors match the largest drain rates, which tells us the cause: we assume the pump always gives 0.36 litres per minute, but it varies with the source bucket. Using the calibrated inlet sensor for volume is the fix.

**34. Command latency** (Uyên)  
Our headline latency is the command round trip, because it is timed by one server clock, so clock skew cancels. The final firmware confirms commands in 29 ms median and 48.5 ms at the 95th percentile, with none lost. The biggest step came from turning off Wi-Fi power saving, which cut the 95th percentile from 884 to 315 ms.

**35. Fault detection** (Uyên)  
When a fault fires, the pump stops in the same 200 ms cycle; we saw that in all 18 overflow and no-progress events. Unplugging the echo wire raised the sensor timeout as designed, and every manual start during a fault was refused. We did not physically trigger NO_CURRENT, so its 2 s is a design value; what we measured is its signal, 353 to 365 mA with the relay on and zero with it off.

**36. Network outage with the pump running** (Uyên)  
This is the test the brief cares most about. We cut the device off the network for 120 seconds while the pump was running at 9 percent. The tank kept filling to 51 percent with no pump change, then stopped itself at 70.7 percent. 125 buffered messages were replayed in order and only 4 were lost, the ones sent in the 3 seconds before the device noticed the link was dead.

**37. Automated system test, 5/5 pass** (Uyên)  
We also wrote an automated acceptance test that anyone can run without an operator. It checks connectivity, security, 40 commands, the right refusals, and the pump current. On the final firmware all five tests passed.

**38. 07 · Lessons and conclusion** (Dương)  
Part seven: lessons and conclusion.

**39. Bugs found on the bench** (Dương)  
Four bugs taught us the most, and each was found by counting something. An inverted relay started the pump on every stop command. The pump started after every reboot because the level was marked valid before any reading. A false echo repeated perfectly and fooled a rule that trusted repeats. And messages were silently cut by a too-small buffer, so everything looked fine while nothing was stored.

**40. Limitations and next steps** (Dương)  
We think stating limits is as important as showing results. The flow sensors are only partly validated. The pump current reads higher than the pump rating and needs one multimeter check. NO_CURRENT has not been triggered physically. The ultrasonic sensor has a blind zone. And the MQTT link inside the lab has no TLS.

**41. Conclusion** (Dương)  
Three ideas carry the project. Control and safety live on the edge, so the tank keeps working without the network, as the outage test showed. Safety is structural: one guard function and seven independent overflow layers. And we trusted measurements over datasheets; every surprise in this project was found by measuring. Now let us show you the real tank.

**42. Live demonstration** (Dương)  
Eight steps on the real tank. We let the level fall and the pump starts and stops by itself; we send a manual command and see the confirmed answer; we lift the float and the pump stops; we lift the intake and the health changes to no flow; we switch off the Wi-Fi and the pump still stops at 70 percent; we pull a pump wire; and we reset the ESP32 to show the pump stays off. A backup video is ready.

**43. Thank you** (Dương)  
Thank you for your attention. We are happy to answer your questions.

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
