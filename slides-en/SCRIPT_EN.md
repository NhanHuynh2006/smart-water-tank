# Presentation script — Smart Water Tank (Project 08)

Full talking script, about 36 minutes at a calm pace, then the 10 to 15 minute live demo. The same text is the speaker notes in the deck: https://claude.ai/artifact/TmihFwubP3Ltox1HqgkGtL. Bracketed items such as [number] are yours to fill.

| # | Slide | Presenter | Time | Ends at |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:40 | 0:40 |
| 2 | Seven parts, then the live demo | Bảo | 0:30 | 1:10 |
| 3 | 01 · Problem and requirements | Bảo | 0:05 | 1:15 |
| 4 | What a smart tank must do | Bảo | 1:00 | 2:15 |
| 5 | Requirements and where they are met | Bảo | 1:05 | 3:20 |
| 6 | 02 · Architecture and hardware | Bảo | 0:05 | 3:25 |
| 7 | System architecture | Bảo | 1:10 | 4:35 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 1:00 | 5:35 |
| 9 | The bench rig | Bảo | 1:05 | 6:40 |
| 10 | The prototype on the bench | Bảo | 0:45 | 7:25 |
| 11 | Controller board schematic | Bảo | 1:05 | 8:30 |
| 12 | Three signal lessons, each found by measuring | Bảo | 1:10 | 9:40 |
| 13 | 03 · MQTT protocol | Nhân | 0:05 | 9:45 |
| 14 | MQTT topic design | Nhân | 1:10 | 10:55 |
| 15 | Commands with acknowledgement | Nhân | 1:05 | 12:00 |
| 16 | 04 · Control and intelligence | Nhân | 0:05 | 12:05 |
| 17 | Level measurement pipeline | Nhân | 1:20 | 13:25 |
| 18 | Model bridging and flow from level | Nhân | 1:00 | 14:25 |
| 19 | Controller state machine | Nhân | 1:20 | 15:45 |
| 20 | Eleven fault rules | Nhân | 1:00 | 16:45 |
| 21 | Safety guard and overflow layers | Nhân | 1:05 | 17:50 |
| 22 | Pump health classification | Nhân | 1:00 | 18:50 |
| 23 | Rolling baseline and daily volume | Nhân | 1:00 | 19:50 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:10 | 20:00 |
| 25 | Backend and data model | Ngân | 1:00 | 21:00 |
| 26 | Dashboard and remote access | Ngân | 1:05 | 22:05 |
| 27 | Security model | Ngân | 1:00 | 23:05 |
| 28 | Behaviour during a network outage | Ngân | 1:00 | 24:05 |
| 29 | 06 · Experiments and results | Uyên | 0:05 | 24:10 |
| 30 | Experiment plan | Uyên | 0:50 | 25:00 |
| 31 | Level calibration against a ruler | Uyên | 0:55 | 25:55 |
| 32 | Control response and switching | Uyên | 0:50 | 26:45 |
| 33 | Volume estimation error | Uyên | 1:00 | 27:45 |
| 34 | Command latency | Uyên | 0:55 | 28:40 |
| 35 | Fault detection | Uyên | 1:00 | 29:40 |
| 36 | Network outage with the pump running | Uyên | 1:05 | 30:45 |
| 37 | Automated system test, 5/5 pass | Uyên | 0:50 | 31:35 |
| 38 | 07 · Lessons and conclusion | Dương | 0:05 | 31:40 |
| 39 | Bugs found on the bench | Dương | 1:00 | 32:40 |
| 40 | Limitations and next steps | Dương | 0:55 | 33:35 |
| 41 | Conclusion | Dương | 0:55 | 34:30 |
| 42 | Live demonstration | Dương | 1:15 | 35:45 |
| 43 | Thank you | Dương | 0:05 | 35:50 |

## 1. Smart Water Tank · Bảo · 0:40

Good morning everyone. We are group [number], and our project is Project 08, the Smart Water Tank. We built a real, small water tank whose pump is controlled by an ESP32. It measures the water level and the flow, it decides by itself when to pump, it detects and classifies faults, and anyone can watch it live from a phone or a computer. If you remember only one idea from this talk, remember this one: every control decision and every safety decision runs on the ESP32 itself. So when the network fails, the tank still keeps working correctly.

## 2. Seven parts, then the live demo · Bảo · 0:30

The talk has seven parts. First, the problem and the requirements of the brief. Second, the architecture and the hardware. Third, the MQTT protocol. Fourth, the core of the project: control and intelligence on the ESP32. Fifth, the backend, the dashboard and security. Sixth, our experiments and measured results. And seventh, the bugs we found, our limitations and the conclusion. After that, we switch to the real tank for a live demonstration.

## 3. 01 · Problem and requirements · Bảo · 0:05

Part one: the problem, and what the assignment asks for.

## 4. What a smart tank must do · Bảo · 1:00

Most houses in Vietnam still use a mechanical float valve. It is cheap and reliable, but it answers only one question: is the tank full? The owner cannot see how much water was used, cannot notice a slow leak, and cannot know that the pump is running dry until it burns out. The brief asks for six things, shown here. Measure level and flow against a real reference. Control the level in a band so the pump does not switch on and off constantly. Protect the tank so it never overflows, even with a broken sensor or a careless manual command. Diagnose faults, such as a pump that runs but moves no water. Supervise everything remotely. And the dark card is the most important one: keep controlling the tank when the network is gone. That last requirement decided our whole architecture.

## 5. Requirements and where they are met · Bảo · 1:05

This table takes each requirement of the brief, says how we meet it, and gives an honest status. It is the same as Table 1.3.1 in the report. Most rows are done and we will show evidence for each of them. There are two rows we want to be upfront about. The first is the flow sensor. Both YF-S401 sensors are fitted, wired and read, and since the fifth of October the inlet sensor counts real water and is calibrated. But the control and the volume counter still use flow derived from the water level, so we call it partly validated, not simply done. The second is the advanced part: we built both options of the brief. The pump health classifier works and we will demo it; the leak baseline is built, but it needs three days of data per half hour before it is allowed to raise an alarm.

## 6. 02 · Architecture and hardware · Bảo · 0:05

Part two: the architecture and the hardware.

## 7. System architecture · Bảo · 1:10

This is the whole system on one slide, and it is the same diagram as in the report. It has four blocks in series. Inside the dashed blue boundary is the edge node: the sensors feed the ESP32, which runs a closed control loop every 200 milliseconds and switches the relay and the pump directly. The ESP32 publishes telemetry, pump state, faults and its last will to a Mosquitto broker on the laptop, and it receives commands on the cmd topic, each one answered on cmd/ack. A FastAPI backend subscribes to the broker, stores everything in six SQLite tables, and serves the web dashboard through a REST API. Finally, a Cloudflare tunnel publishes the dashboard on a public HTTPS address, so anyone can watch from anywhere without opening a port on our network. Please notice the dark box: the only path to the pump goes through the ESP32. The server can ask, but it can never switch the pump by itself.

## 8. The server never switches the pump. It asks, and the ESP32 decides. · Bảo · 1:00

This is the single most important decision in the project. The lectures give a rule: any operation that needs an immediate safe stop must be computed at the edge, close to the hardware. We applied that rule strictly. Everything that needs a fixed cycle or must survive an outage lives on the ESP32: the level filter, the state machine, all eleven fault rules, the safety guard, the pump health classifier and the volume counter. The server keeps only what the microcontroller cannot do well: long history, the daily volume report, and a leak baseline that needs several days of memory. The result is that the system degrades gracefully. When the network drops, the user loses the view and the remote buttons, but the tank still refills at the correct thresholds and every safety rule still works.

## 9. The bench rig · Bảo · 1:05

Our tank is deliberately small. The base is 10 by 10 centimetres, so one centimetre of water is exactly 0.1 litre, which makes volume easy to check. The sensor is 15.88 centimetres above the floor and the working range is 12.4 centimetres, so the tank holds 1.24 litres, and one fill and drain cycle takes only a few minutes. The orange number is the most important measurement here. The pump datasheet promises 1.67 litres per minute. We measured the real flow by closing the outlet and timing the level: only 0.36 litres per minute, five times less, because of the height and the tubing. On the fifth of October, with a lower source bucket, it was even 0.24. So every timing threshold in our firmware is based on measured numbers, never on the datasheet. The table lists each sensor and how its signal is prepared for the 3.3 volt inputs of the ESP32.

## 10. The prototype on the bench · Bảo · 0:45

This is the real prototype on the bench. The transparent main tank has the ultrasonic sensor and the upper float switch in its lid. The water runs in a closed loop. The pump sits in the source bucket and pushes water through the inlet flow sensor into the main tank. The water leaves the tank through a manual valve, which plays the role of the household consumption, and then through the outlet flow sensor back to the bucket. The controller board with the ESP32 sits on the lid of the source bucket. Everything you will see in the demo happens on this rig.

## 11. Controller board schematic · Bảo · 1:05

This is the schematic of our controller board, drawn in KiCad. A 12 volt supply comes in on a screw terminal, and two step-down modules produce two rails: 5 volts and 3.3 volts. The 5 volt rail powers the ESP32, the ultrasonic sensor, both flow sensors and the current sensor, with a 470 microfarad capacitor to absorb the motor's start current. Every sensor output on that rail is a 5 volt signal, but the ESP32 only accepts 3.3 volts, so each one goes through a 10k and 20k divider. The 3.3 volt rail powers the relay module, the two float switches and the reset button. One detail: the relay is on GPIO 10, a pin that is normally used by the flash memory, so we set the flash to DIO mode to free it, and we verified on the bench that it works without resetting the chip.

## 12. Three signal lessons, each found by measuring · Bảo · 1:10

These are three wiring lessons, and each one was found by measuring, not by reading code. First, the echo line originally went through an automatic level shifter. That chip is made for two-way buses, and it distorted the echo pulse, whose width is the measurement itself. We lost about 30 percent of the readings; a simple resistor divider fixed it completely. Second, a flow input reported water flowing while the valve was closed. The frequency was exactly 50 hertz, the mains frequency: the input was floating and acting as an antenna. Third, while the pump ran, both flow lines showed about 1500 hertz of noise, forty times the real signal. A diode and moving the wires did not help, which told us the noise travelled through the power supply. Only after we rebuilt the power stage on the fifth of October did the inlet sensor count real water. It is now calibrated, but we still say clearly that control uses flow derived from the level.

## 13. 03 · MQTT protocol · Nhân · 0:05

Part three: the MQTT protocol.

## 14. MQTT topic design · Nhân · 1:10

Our topic tree goes from general to specific: water tank, site, device. For each topic we chose the quality of service by asking one question: what does it cost to lose one message? Telemetry is a stream at one message per second, so losing one sample changes nothing, and it uses QoS 0. The pump state is sent only when it changes, so it uses QoS 1 and is retained, which means a dashboard that opens later receives the current state immediately. Faults, commands and acknowledgements also use QoS 1, because losing one of them is a real failure. The status topic carries the last will: if the ESP32 disappears without saying goodbye, the broker itself announces that it is offline. Every message also carries a sequence number, which is how we count lost messages exactly. Finally, the broker has an access control list, so even if someone steals the device's account, it still cannot publish pump commands.

## 15. Commands with acknowledgement · Nhân · 1:05

This diagram follows one press of the Pump ON button. The dashboard sends the command to the backend with the administrator's session. The backend gives it a unique identifier, publishes it to the broker and answers 'pending'. At this point the button has not changed; the screen only says 'sending'. The ESP32 receives the command and decides: is it in manual mode, does the safety guard allow it, has the minimum rest time passed? Then it publishes an acknowledgement with the result, the reason and the real pump state. The dashboard shows that answer as a plain sentence, and the pump symbol follows only what the device reports. This is the feedback principle from the lectures: the screen shows what the machine confirmed, not what the user wished. When the device refuses, for example inside the 20 second rest, the user sees the reason and a countdown instead of a button that seems broken.

## 16. 04 · Control and intelligence · Nhân · 0:05

Part four, the core of the project: control and intelligence on the ESP32.

## 17. Level measurement pipeline · Nhân · 1:20

Measuring the level sounds simple: the level is the sensor height minus the measured distance. But the ultrasonic beam is wider than our tank, so some echoes come back from the wall or the floor. That is why we never trust a single ping. Every reading passes five steps, shown left to right. We keep a window of the last fifteen pings and drop anything farther than the floor. If the spread of the window is too large, the sensor is jumping between surfaces and we reject the window. Otherwise we take the lower quartile, because late echoes only ever make the distance too long. Then a physics check rejects changes faster than the pump can cause. Finally an alpha-beta filter tracks both the level and its rate of change, without the delay of a moving average. Counting rejections at each step showed that one badly chosen statistic was discarding most readings; replacing it raised acceptance from 60 to 93 percent. And in parallel, on the bottom row, the raw distance is checked on every ping: three readings closer than 4.5 centimetres trigger the overflow protection, whatever the filter says.

## 18. Model bridging and flow from level · Nhân · 1:00

Next to the sensor, the ESP32 runs a simple model of the tank: the level changes by inflow minus outflow, divided by the base area. The model is gently pulled towards every good reading, so it never drifts far. Its first job is to bridge gaps. Sometimes the sensor gives no trusted reading for tens of seconds; our first firmware simply stopped every such fill at about 50 percent. Now the state machine controls on the model during the gap, but for at most 90 seconds; after that the pump stops and we raise LEVEL_LOST, so the model never replaces the sensor for long. Its second job is the outflow shown on the dashboard. We measure outflow only when the pump has been off for twenty seconds and hold that value while pumping, because the valve, not the pump, decides how much water is used.

## 19. Controller state machine · Nhân · 1:20

The controller is a state machine with seven states. At boot, the very first instruction switches the relay off, and the device waits for a real reading before doing anything. Then it waits in IDLE. In automatic mode, when the level falls below 30 percent, or the lower float closes, it starts the pump and enters FILLING; above 70 percent it stops and goes back to IDLE. That band from 30 to 70 percent is the hysteresis, and together with a minimum on time of 3 seconds and a minimum off time of 20 seconds, it stops the pump from chattering. The line on top is the manual path: an operator command goes to MANUAL_ON, but only if the safety guard allows it. Any fault rule stops the pump and moves to a fault state. Why a state machine and not a simple if statement? Because faults must be sticky. A sensor fault clears itself after the signal is good again, but a pump fault waits for a person, otherwise a pump sucking air would restart the moment the hose touches water.

## 20. Eleven fault rules · Nhân · 1:00

The brief asks for at least one fault rule; we implemented eleven. They are checked every 200 milliseconds, in order of priority, and any rule that fires stops the pump at once and sends a fault message. The first three are about the signal and clear themselves when the signal comes back. The others lock the pump until an operator clears them, except the leak warning. Two rows are highlighted. OVERFLOW has three independent triggers: the mechanical float, the raw distance, and the filtered level. NO_PROGRESS is the physical form of the brief's example rule, 'pump on with almost no flow': the pump draws current but the level does not rise. The sensor timeout is long on purpose, 90 seconds while the model is valid, because shorter limits stopped normal fills. During that wait, the float and the raw distance check still protect the tank.

## 21. Safety guard and overflow layers · Nhân · 1:05

The brief says manual mode must not bypass safety. We solved that with structure, not with discipline. Every code path that can start the pump, automatic or manual, goes through one function that either allows it or returns the reason for refusing: an active fault, the upper float, water too close to the sensor, the overflow limit, an untrusted level, or the minimum rest time. While a manual pump runs, the same function is checked again every cycle, so lifting the float stops the pump within 200 milliseconds. To check our safety, a reviewer reads one function instead of the whole code. On the right are seven independent layers against overflow. Some depend on the filtered level, but others do not: the raw distance check, the time and volume limits, the mechanical float, and switching the relay off at boot. No single failure can disable all of them.

## 22. Pump health classification · Nhân · 1:00

The brief offers two advanced options and we built both. The first is classifying pump faults by comparing pump current with water movement. If there is current and the water is moving, the pump is healthy. If there is current but no water moves after 25 seconds, the label is no_flow: a hydraulic problem, such as a dry intake or a blocked pipe. If the relay is closed but there is no current, the label is no_current: an electrical problem, such as a broken wire, a bad relay or a dead motor. So the same symptom, a tank that does not fill, is split into two causes that send the technician to two different places. The label is saved with every fault, and in the demo we will lift the intake out of the water to show it change.

## 23. Rolling baseline and daily volume · Nhân · 1:00

The second advanced option is detecting abnormal consumption, and it runs on the backend because it needs days of memory. Water use depends strongly on the time of day, so one fixed threshold would be too sensitive at night and too loose in the evening. We cut the day into 48 half-hour slots, and each slot learns its own normal use. An alarm is raised when use stays above the mean plus three standard deviations for two slots in a row, which ignores one-off events like washing a motorbike. On the right is the daily volume report, pumped in versus used. To be honest: each slot needs three days of data before it is allowed to alarm, and we do not have that yet, so we can show the mechanism but not a real detection.

## 24. 05 · Backend, dashboard, security · Ngân · 0:10

Part five: the platform around the device. That means storage, the dashboard, security, and what happens when the network fails.

## 25. Backend and data model · Ngân · 1:00

The backend is a single Python process. It subscribes to the MQTT topics, writes each message into one of six SQLite tables, and serves a REST API for the dashboard. We chose SQLite on purpose. The lectures explain that relational databases struggle with time series data, but that happens at thousands of writes per second. We write one row per second from one device, so a dedicated time series database would only add work. We still follow the design rules: only the timestamp is indexed, not the sequence number. For the API, reading data uses GET because it changes nothing, the configuration uses PUT because sending it twice gives the same result, and commands use POST because each one is new. The two highlighted endpoints change the physical world, so they require an administrator session.

## 26. Dashboard and remote access · Ngân · 1:05

This is our dashboard, on a computer on the left and on a phone on the right. It is one single web page: on a phone, the panels stack into one column, so the same link works everywhere. At the top is a drawing of the real system, with the water drawn at its measured height and the 30 and 70 percent lines at their true positions, so anyone can see why the pump just switched. Below are the numbers, the history charts and the logs. The page is public through a Cloudflare tunnel, so a phone on mobile data can watch the tank. But control is private: the buttons appear only after logging in with the administrator password. We first tried to allow control only from the laptop's address, and dropped it, because through the tunnel every request appears to come from the laptop itself, which would have given control to the whole Internet.

## 27. Security model · Ngân · 1:00

We analysed security layer by layer, as the lectures suggest. At the broker, anonymous access is off and an access control list limits each account. At the API, every endpoint that moves the pump needs a session; the password is stored outside the code, the session cookie cannot be read by scripts, and five wrong passwords lock that address. The two blue rows are interesting, because the control design itself stops them: fake data cannot cause an overflow because the float works on the device, and spamming commands cannot wear out the pump because the minimum on and off times are in the firmware. The orange row is our open gap, and we state it plainly: MQTT inside the lab is not encrypted. So we do not claim end-to-end security; TLS on port 8883 is the step for a real deployment.

## 28. Behaviour during a network outage · Ngân · 1:00

What happens when the network fails? Four mechanisms. First, the control loop never waits for the network. Opening a connection to a broker that does not answer normally blocks for seconds, so we limit it to half a second, and we close a dead connection after three seconds; the device even reports its longest loop time so this can be checked. Second, nothing is forgotten: while offline, telemetry goes into a ring buffer of 240 samples, about four minutes, and is replayed in order when the link returns. Third, everyone knows: the broker publishes the last will, and the dashboard turns red after five seconds without data. Fourth, the device finds its way back by itself: it retries with growing delays, can use up to three Wi-Fi networks, and finds the laptop again by its name.

## 29. 06 · Experiments and results · Uyên · 0:05

Part six: our experiments and results.

## 30. Experiment plan · Uyên · 0:50

Our rule for experiments was simple. Every measurement needs a reference that is independent of the system being measured, and every number must come from the database through a script that anyone can run again. This table lists the experiments, the reference for each, and the result. We analysed two sessions. The control session on the twenty third of September lasted 84 minutes with almost five thousand messages and not a single false fault. The calibration and acceptance session on the fifth of October used the final firmware and the final sensor position. One honest note: the no-current fault was not triggered by physically pulling a wire; we explain what we measured instead on the E5 slide.

## 31. Level calibration against a ruler · Uyên · 0:55

Experiment one is the level calibration. With the drain valve closed, we pumped water in steps of 20 seconds. After each step the water settled, and we took 40 readings. The inlet sensor counted the water pumped, so the true level after each step is one ruler reading plus the pumped volume divided by the base area. The result: outside one band, the sensor agrees with the reference within 0.68 centimetres. But inside that band, when the water surface is about 8 centimetres below the sensor, all 40 readings return the same wrong value, a stray echo. Because it is perfectly stable, it cannot be removed as noise. This explained why our fills used to stop around 60 percent. The firmware now rejects these readings and crosses the band using the tank model.

## 32. Control response and switching · Uyên · 0:50

This chart shows 76 minutes of real operation on the bench. Blue bands are the pump running; hatched areas are manual mode, which we used for tests. In automatic mode, every fill stopped between 70.1 and 70.8 percent against a 70 percent threshold. The level then rises a little more, 1.4 percent on average, because water still in the pipe keeps arriving after the relay opens; even the highest peak leaves 12 points of margin to the overflow line. The pump started about 13 times per hour, and the shortest rest was 32 seconds, longer than our 20 second minimum, so the relay never chattered. And not one fault rule fired falsely in the whole session.

## 33. Volume estimation error · Uyên · 1:00

How accurate is the accumulated volume? For each automatic fill we compare the device's pumped volume with a reference built only from the tank: the level rise times the base area, plus the water that drained out during the fill. The drain rate is measured separately from the level slope with the pump off, before and after each fill. Over six fills, the average error is minus 5.2 percent, from minus 15 to plus 8. The pattern is informative: the largest errors happen when the drain is fastest. That is exactly what we expect, because the device assumes the pump always gives 0.36 litres per minute, while the real flow changes with the water in the source bucket. The calibrated inlet sensor is the fix, and moving the volume counter to it is our next step.

## 34. Command latency · Uyên · 0:55

For latency, our headline number is the command round trip, and the reason is about measurement. A one-way delay subtracts two clocks, the server's and the ESP32's, and the clock on a microcontroller is only accurate to tens of milliseconds, the same size as what we want to measure. The round trip is timed by the server at both ends, so the clock error cancels. The chart shows how it improved. The first firmware had a median of 226 milliseconds. The biggest step was the radio: by default the ESP32 lets its Wi-Fi sleep between beacons, so messages wait. Turning that off cut the 95th percentile from 884 to 315 milliseconds. On the final firmware, forty commands were confirmed with a median of 29 milliseconds and none was lost.

## 35. Fault detection · Uyên · 1:00

This table shows fault detection. When we unplugged the echo wire, the sensor timeout fired as designed. In all eighteen overflow and no-progress events where the pump was running, the pump stopped in the same 200 millisecond cycle as the fault. And every manual start attempted during a fault was refused with the right reason. We want to be clear about the orange row: we did not physically pull a pump wire to trigger NO_CURRENT, so its 2 seconds is a design value, not a measurement. What we did measure is the signal it depends on: with the relay closed, the current was 353 to 365 milliamps in every message, and zero when it opened. We also saw NO_PROGRESS fire for real when the drain valve was opened so wide that the pump could not raise the level.

## 36. Network outage with the pump running · Uyên · 1:05

This is the test the brief cares about most, run on the final firmware with the pump running. We blocked all of the device's traffic for 120 seconds, while the broker and the backend stayed up. The grey band is the outage, and the tank was at 9 percent when it started. The orange line is what the ESP32 recorded while offline: it kept filling, from 9 to 51 percent, without any change of pump state. After the link came back, those 125 messages were replayed in order. The grey dashed part is the sensor's blind zone, where the model takes over. Then the device stopped the pump by itself at 70.7 percent. Only 4 of 129 messages were lost: the ones sent in the three seconds before the device noticed the connection was dead. The control loop never paused for more than 0.88 seconds, and the device was back online 10 seconds after the network returned.

## 37. Automated system test, 5/5 pass · Uyên · 0:50

Before the defence, we wanted a test that anyone can run again without an operator, so we wrote an automated system test. It ran on the final firmware and all five tests passed. Test one listens for ten seconds and checks the message rate, missing sequence numbers and required fields. Test two checks security: anyone can view, but a command without a session or with a wrong password is refused. Test three sends forty commands and measures the round trip; none was lost. Test four checks that the device refuses what it must, with the correct reason each time. Test five runs the pump by hand for eight seconds and reads the current: about 360 milliamps while running and zero after.

## 38. 07 · Lessons and conclusion · Dương · 0:05

Part seven: lessons, limitations, the conclusion, and then the demo.

## 39. Bugs found on the bench · Dương · 1:00

Four bugs taught us the most. Each was invisible in the code and was found by counting something. First, an inverted relay: the settings assumed the relay was active low, but it was active high, so every stop command started the pump while the screen said 'off'. We proved the real state from the ripple of the motor current. Second, the pump started after every reboot, because for a few seconds the level was treated as valid at zero percent, and the controller saw an empty tank. We fixed it, and on the fifth of October a restart at 33 percent kept the pump off. Third, a false echo repeated perfectly and fooled a rule that trusted repeated values; we removed that rule. Fourth, messages were silently cut by a buffer that was too small, so everything looked alive while nothing was stored.

## 40. Limitations and next steps · Dương · 0:55

We believe stating our limits clearly is as valuable as showing results. The main open item is flow measurement: the sensors are fitted and the inlet is calibrated, but it was validated on one run only, so control and the volume counter still use flow from the level; the next step is to validate it over full cycles and switch the counter to it. Second, the pump current reads higher than the pump's rating, so one check with a multimeter is needed. Third, the no-current fault was never triggered physically; we will do it in the demo. Fourth, the ultrasonic sensor has a blind zone, which foam around the sensor or a higher mount would remove. And for a real deployment, MQTT needs TLS, a fixed public address, personal accounts and over-the-air updates.

## 41. Conclusion · Dương · 0:55

To conclude, three ideas carry the project. First, the whole control law and every safety rule run on the ESP32, so working without the network is a property of the architecture, not an extra feature; the outage test showed the tank filling and stopping by itself with no network. Second, safety is structural: one guard function that every pump start must pass, and seven independent layers against overflow. Third, we trusted measurements over datasheets and over our own expectations. The pump gives a fifth of its rated flow, the flow sensors counted noise until we rebuilt the power stage, the level sensor has a blind zone, and our own data revealed a reboot bug. Every one of these was found by measuring. Now let us show you the real tank.

## 42. Live demonstration · Dương · 1:15

Now the live demo, eight steps on the real tank. One: we open the drain and you see the level fall, on the projector and on a phone over mobile data. Two: below 30 percent the pump starts by itself and stops at 70. Three: we send a manual command and watch 'sending' turn into the device's confirmed answer. Four: with the pump on, we lift the upper float; the pump stops at once and the fault cannot be cleared while the float is up. Five: we lift the pump intake out of the water and the health label changes to no flow. Six: we switch off the Wi-Fi during a fill; the dashboard turns red, but the pump still stops at 70 percent by itself, and the missing data fills in when the network returns. Seven: we pull one pump wire and see NO_CURRENT. Eight: we reset the ESP32 above 30 percent and the pump stays off. If the hardware misbehaves, we have a backup video of the same steps.

## 43. Thank you · Dương · 0:05

Thank you very much for your attention. We are happy to answer your questions.

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
