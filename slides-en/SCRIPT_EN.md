# Presentation script — Smart Water Tank (Project 08)

English script for the technical part (about 38 minutes at a calm 145 words per minute), followed by the 10 to 15 minute live demo. The same text is stored as the speaker notes of each slide in the deck: https://claude.ai/artifact/TmihFwubP3Ltox1HqgkGtL

Tips: speak to the room, not to the slide; pause on the accent slides (8 and 33); on dividers just say the one line and move on. Bracketed items, such as [group number], are for you to fill in.

| # | Slide | Time | Ends at |
|---|---|---|---|
| 1 | Smart Water Tank | 0:45 | 0:45 |
| 2 | Seven parts, then the live demo | 0:30 | 1:15 |
| 3 | Problem and requirements | 0:10 | 1:25 |
| 4 | What a smart tank must do | 1:05 | 2:30 |
| 5 | Requirements and where they are met | 0:50 | 3:20 |
| 6 | Architecture and hardware | 0:10 | 3:30 |
| 7 | System architecture | 1:05 | 4:35 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | 1:00 | 5:35 |
| 9 | The bench rig | 1:00 | 6:35 |
| 10 | Three wiring lessons, each found by measuring | 1:25 | 8:00 |
| 11 | MQTT protocol | 0:10 | 8:10 |
| 12 | MQTT topic design | 1:10 | 9:20 |
| 13 | Commands with acknowledgement | 1:30 | 10:50 |
| 14 | Control and intelligence | 0:10 | 11:00 |
| 15 | Level measurement pipeline | 1:35 | 12:35 |
| 16 | Model bridging and flow from level | 1:30 | 14:05 |
| 17 | Controller state machine | 1:20 | 15:25 |
| 18 | Eleven fault rules | 1:20 | 16:45 |
| 19 | Safety guard and overflow layers | 1:15 | 18:00 |
| 20 | Pump health classification | 1:25 | 19:25 |
| 21 | Rolling baseline and daily volume | 1:25 | 20:50 |
| 22 | Backend, dashboard, security | 0:10 | 21:00 |
| 23 | Backend and data model | 1:05 | 22:05 |
| 24 | Dashboard and remote access | 1:20 | 23:25 |
| 25 | Security model | 1:20 | 24:45 |
| 26 | Behaviour during a network outage | 1:20 | 26:05 |
| 27 | Experiments and results | 0:10 | 26:15 |
| 28 | Experiment plan | 0:55 | 27:10 |
| 29 | Control response and switching | 1:30 | 28:40 |
| 30 | Volume estimation error | 1:10 | 29:50 |
| 31 | Command latency | 1:20 | 31:10 |
| 32 | Fault detection and network outage | 1:25 | 32:35 |
| 33 | The result to remember: 70.4 % | 0:25 | 33:00 |
| 34 | Lessons and conclusion | 0:10 | 33:10 |
| 35 | Bugs found on the bench | 1:20 | 34:30 |
| 36 | Limitations and next steps | 1:20 | 35:50 |
| 37 | Conclusion | 1:10 | 37:00 |
| 38 | Live demonstration | 1:20 | 38:20 |
| 39 | Thank you | 0:15 | 38:35 |

Total spoken time: about 38 min 35 s.

## 1. Smart Water Tank  ·  0:45

Good morning everyone. We are group [number], and our project is Project 08, the Smart Water Tank. In one sentence: we built a small real water tank whose pump is controlled by an ESP32, which measures level and consumption, detects faults, and can be supervised from anywhere through MQTT and a web dashboard. If you remember one idea from this talk, it is this: every control decision and every safety decision is made on the ESP32 itself, so the tank keeps working even when the network does not. The talk has seven parts and takes about forty minutes, followed by a live demonstration on the real tank.

## 2. Seven parts, then the live demo  ·  0:30

Here is the plan. First, the problem and what the assignment asks for. Second, the architecture and the hardware, including the single most important design decision. Third, the MQTT protocol. Fourth, the core of the project: control and intelligence on the ESP32. Fifth, the backend, the dashboard and security. Sixth, our experiments and measured results. Seventh, the bugs we found, the limitations, and the conclusion. Then we switch to the real tank for the live demo.

## 3. Problem and requirements  ·  0:10

Part one: the problem, and the requirements of the assignment.

## 4. What a smart tank must do  ·  1:05

Most houses in Vietnam still rely on a mechanical float valve in the storage tank. It is cheap and reliable, but it answers exactly one question: is the tank full? The owner cannot see how much water was used, cannot notice a slow leak until the bill arrives, and cannot know that the pump is running dry until it burns out. The assignment asks for six things, shown here. We must measure level and flow against a real reference. We must control the level with a band so the pump does not switch on and off constantly. We must protect the tank so it never overflows, even with a broken sensor or a careless manual command. We must diagnose faults, for example a pump that runs but moves no water. We must supervise everything remotely. And the dark card is the overriding requirement: the tank must keep controlling itself when the network is gone. That last requirement shaped our whole architecture.

## 5. Requirements and where they are met  ·  0:50

This table takes each requirement of the brief, says how we meet it, and gives an honest status. Most rows are met and demonstrated. Two points I want to be upfront about, because we will come back to both with measurements. First, the flow sensors are physically fitted and wired, but in our installation the numbers they produce cannot be trusted, so the flow used for control is derived from the level. Second, for the advanced component we built both options the brief offers: a rolling baseline for abnormal consumption, and a classifier based on pump current versus flow. The classifier works and we will demo it; the baseline needs several days of data before it is allowed to raise an alarm.

## 6. Architecture and hardware  ·  0:10

Part two: the architecture and the hardware.

## 7. System architecture  ·  1:05

This is the whole system on one slide. On the left, inside the edge node boundary, the sensors feed the ESP32 firmware, which runs a closed control loop every 200 milliseconds and drives the relay and the pump directly. The ESP32 publishes over MQTT to a Mosquitto broker on the laptop. A FastAPI backend subscribes, stores everything in SQLite and serves the web dashboard through a REST API. Finally, a Cloudflare tunnel publishes the dashboard on a public HTTPS address, so anyone with the link can watch the tank from anywhere, without opening an inbound port on our network. Mapped onto the five layer model from the lectures: perception is the ESP32 and its sensors, transport is MQTT over Wi-Fi, processing is FastAPI and SQLite, and application is the dashboard. Notice that the only path from the server to the pump goes through the ESP32 itself. The server can ask; it can never force.

## 8. The server never switches the pump. It asks, and the ESP32 decides.  ·  1:00

This is the single most important decision in the project. The lectures give a placement rule: any operation that needs an immediate safe stop must be computed at the edge. We applied it strictly. Everything that needs a deterministic cycle or has to survive an outage lives on the ESP32: filtering, the state machine, all eleven fault rules, the safety guard, the pump health classifier and the volume accumulation. The server keeps only what the microcontroller cannot do well: long history, the daily report, and a leak baseline that needs days of memory. The consequence is graceful degradation. When the network drops, the user loses visibility and remote commands, but the tank still refills at the correct thresholds and every safety rule still applies. The outage requirement becomes a property of the architecture, not a feature we bolted on.

## 9. The bench rig  ·  1:00

Our tank is deliberately small: a ten by ten centimetre base, so one centimetre of level is exactly a tenth of a litre, and a working range of fourteen centimetres, or 1.4 litres. A full fill and drain cycle takes about four minutes, so we can observe dozens of control cycles in one session. The orange number is the most important measurement on this slide. The pump datasheet promises 1.67 litres per minute. We measured the real delivery by closing the outlet and timing the level: 1.2 centimetres in 20 seconds, which is 0.36 litres per minute, almost five times less, because of the delivery head and the tubing. Every timing threshold in our firmware is sized from that measured number, not from the datasheet. The table lists the sensors and how each signal is conditioned for the 3.3 volt inputs of the ESP32.

## 10. Three wiring lessons, each found by measuring  ·  1:25

Three wiring lessons, each found by measuring rather than by reading code. First, the ultrasonic echo line originally went through a TXS0108E automatic level shifter. That part is designed for open drain buses, and its pull ups and edge accelerators distorted the echo pulse, whose width is the measurement. We lost about thirty percent of the echoes. A plain resistor divider fixed it completely: zero misses in 179 pings. Second, the flow inputs reported flow with the valve closed. The pulse rate was exactly 50.00 hertz, the mains frequency: the weak internal pull up turned the wire into an antenna. External 4.7 kilo-ohm pull ups brought it to zero. Third, and this one we could not fix in time: with the pump running, both flow lines read about 1500 hertz, while real water at 0.36 litres per minute gives about 35 hertz. A flyback diode and moving the wires apart changed nothing, so the noise is conducted through the shared supply and ground, not radiated. On top of that, our outlet flow is below the sensor's minimum of 0.3 litres per minute. So for control and accounting we derive flow from the level, and the dashboard labels it as such.

## 11. MQTT protocol  ·  0:10

Part three: the communication protocol.

## 12. MQTT topic design  ·  1:10

Our topic tree goes from broad to specific: water tank, site, device. We chose the quality of service per topic by asking one question: what does it cost to lose one message? Telemetry is a one hertz stream, so losing one sample changes no conclusion, and it uses QoS zero. The pump state is published only when it changes, so it uses QoS one and is retained, which means a dashboard that opens later immediately receives the current state. Faults, commands and acknowledgements use QoS one because losing any of them is a functional or safety failure. The status topic carries the last will message: if the ESP32 disappears without a clean disconnect, the broker itself publishes "offline". We never use QoS two; duplicate commands are already handled by a unique command identifier, so the device just repeats the same acknowledgement. Every message carries a sequence number and a millisecond timestamp, which is how we count lost messages exactly. And the broker has an access control list: even a compromised ESP32 cannot publish pump commands.

## 13. Commands with acknowledgement  ·  1:30

This diagram follows one press of the Pump ON button. The dashboard posts the command to the backend with the administrator's session cookie. The backend logs it with a fresh command identifier, publishes it on the command topic, and immediately answers "pending". At this point the button has not changed; the screen only says "sending". The ESP32 receives the command and decides: is it in manual mode, does the safety guard allow a start, has the minimum off time elapsed? It then publishes an acknowledgement carrying the outcome, the reason and the real pump state. The dashboard polls the command log and shows the result as a plain sentence, while the pump symbol follows only the telemetry. This is the feedback sync principle from the lectures. Two lessons came from this path. First, a user once reported that manual mode did not work; in fact the device was correctly refusing a restart inside the twenty second rest period, and the interface was silent about it. Now it shows the reason and a countdown. Second, pump commands were executed but never confirmed: the MQTT library reuses one buffer for incoming and outgoing messages, and the JSON parser was reading in place, so publishing the pump state overwrote the command identifier. We now copy the payload before parsing.

## 14. Control and intelligence  ·  0:10

Part four, the core of the project: control and intelligence on the ESP32.

## 15. Level measurement pipeline  ·  1:35

Now the core. The ultrasonic sensor sits 17.5 centimetres above the tank floor, so the level is 17.5 minus the measured distance. That sounds trivial, but the sensor's beam spreads about fifteen degrees and is already wider than our ten centimetre tank when it reaches the water, so some echoes come back from the wall or the floor. We therefore never trust a single ping. Each reading passes five gates. We keep a window of the last fifteen pings. If the interquartile range of the window is larger than 3.5 centimetres, the sensor is jumping between two surfaces, and we reject the window. Otherwise we take the median. Then a physics check rejects changes faster than the pump could cause, or a falling level while the pump is filling. Finally an alpha-beta filter tracks both the level and its rate of change, without the lag of a moving average. Every gate has its own rejection counter in the telemetry. That is how we found that ninety-eight percent of rejections came from one gate, which used maximum minus minimum; one stray echo controls that statistic completely. Switching to the interquartile range raised acceptance from sixty to ninety-three percent. And separately, for safety, a raw distance check bypasses all of this filtering: three pings closer than 4.5 centimetres trigger the overflow protection, whatever the filter believes.

## 16. Model bridging and flow from level  ·  1:30

We also run a simple model of the tank in parallel with the measurement. The level changes by inflow minus outflow, divided by the base area. The model is pulled gently towards every accepted reading, so it never drifts far. Its first use is bridging: during a fill the ultrasonic sensor sometimes produces no accepted reading for tens of seconds, and our first firmware simply stopped every such fill at about fifty percent. Now the state machine controls on the model during the gap, for at most forty-five seconds; after that, the pump stops and we raise LEVEL_LOST. The model never replaces the sensor for long. Its second use is flow. Because the turbine sensors are unusable here, inflow is the calibrated pump delivery while the pump runs, and outflow is the falling rate of the level times the area. One subtlety: the first version computed outflow as pump delivery minus the net rise at every instant. The pump term switches instantly but the rate estimate lags, so every pump start made the outflow curve jump on the dashboard, as if consumption followed the pump. Physically, the valve sets the draw, not the pump. So we measure outflow only when the pump has been off for twenty seconds, and hold that value while pumping.

## 17. Controller state machine  ·  1:20

The controller is a state machine with seven states. After boot, where the very first instruction drives the relay off, the device waits in IDLE. In automatic mode, when the level falls below thirty percent, or the low float engages, it starts the pump and enters FILLING. Above seventy percent it stops and returns to IDLE. That band from thirty to seventy is our hysteresis, and together with a minimum on time of three seconds and a minimum off time of twenty seconds it prevents chattering. MANUAL_ON is entered only by an operator command. Any fault rule, from FILLING or from MANUAL_ON, stops the pump immediately and moves to one of three fault states. Why a state machine and not a simple if statement? Because fault states must be sticky. Sensor faults clear themselves after two and a half seconds of good signal, shown by the dashed arrow. But a dry run or an overflow lock waits for an operator, otherwise a pump that sucked air would restart the moment the hose touches water again. One invariant is enforced on every cycle: if a fault code is set, the state must be a fault state.

## 18. Eleven fault rules  ·  1:20

The brief asks for at least one fault rule; we implemented eleven. They run every two hundred milliseconds, in priority order, and any rule that fires stops the pump at once and publishes a fault message. The first three concern the signal and clear themselves when the signal returns. The others lock the pump until an operator clears them, except the leak warning. Two rows are highlighted. OVERFLOW has three independent triggers: the mechanical float, the raw distance, or the filtered level. NO_PROGRESS is the physical form of the brief's example rule, "pump on with near zero flow". Its literal form, DRY_RUN, uses the turbine signal, and since we do not trust that signal in this installation, DRY_RUN is disabled when flow is derived from level. A guard that can never fire is worse than none. NO_PROGRESS uses a quantity we do measure: the pump draws current but the level does not rise. The forty-five second sensor timeout is long on purpose: the sensor drops out for tens of seconds during normal fills and the model covers that gap. Our first version used four seconds and stopped every fill early.

## 19. Safety guard and overflow layers  ·  1:15

The brief says manual mode must not bypass safety. We solved that with structure, not discipline. Every code path that can start the pump, automatic or manual, goes through one function, pumpBlockReason, which returns either nothing or the reason for refusing: an active fault, the upper float, water too close to the sensor, the overflow limit, an untrusted level, or the minimum off time. While a manual pump is running, the same function is re-evaluated every two hundred milliseconds, so lifting the float stops it within one cycle. To verify safety, a reviewer reads one function instead of the whole codebase. On the right are the independent overflow layers. The first two rely on the filtered level and fail together if the sensor fails. The raw distance check bypasses the filter. The progress rule catches a frozen reading. The volume and time caps work even with every level sensor lost, although honestly they only bound the damage. The mechanical float needs no measurement at all, and the relay is driven off as the very first instruction at boot. No single failure disables overflow protection.

## 20. Pump health classification  ·  1:25

The brief offers two options for the advanced component, and we built both. The first is pump health classification, using the consistency between pump current and water flow. When the pump is on and current flows and the water is moving, the label is ok. When current flows but no water moves after twenty-five seconds, the label is no_flow, which points to the hydraulic side: a dry intake, a blocked or detached pipe. When the relay is closed but no current flows, the label is no_current, which points to the electrical side: an open wire, a failed relay or a dead motor. The same symptom, a tank that does not fill, is split into two causes that send the technician to two different places. Evidence of water is the level rising faster than one hundredth of a centimetre per second; the real rise is about 0.036, so twenty-five seconds of pumping gives almost a centimetre, well above the filter noise. The label is diagnostic only; stopping the pump is still the job of the fault rules. But it is captured at the moment of every fault and stored with it. In the demo we will lift the intake out of the water and watch it change to no_flow.

## 21. Rolling baseline and daily volume  ·  1:25

The second advanced option is anomaly detection with a rolling baseline, which runs on the backend because it needs days of memory. Domestic water use depends strongly on the hour, so a single threshold would be too sensitive at night and too lax in the evening. We cut the day into forty-eight half-hour slots. Each slot learns its own normal consumption with an exponentially weighted mean and variance. An alarm is raised when consumption exceeds the mean plus three standard deviations in two consecutive slots, which suppresses one-off events like washing a motorbike. We learned two safeguards the hard way: early noisy flow readings had accumulated thousands of phantom litres, so slots with a counter reset or a physically impossible volume are skipped, and the baseline is rebuilt from the database whenever the backend restarts. Honest status: a slot needs three days of data before it may alarm, and after three bench days none qualifies yet, so we can show the mechanism but not a real detection. On the right is the daily volume report from the API, pumped in versus drawn out. The query sums the increments between consecutive rows, so a counter reset adds nothing instead of a huge negative step.

## 22. Backend, dashboard, security  ·  0:10

Part five: the platform around the device, meaning storage, the dashboard, security, and outage behaviour.

## 23. Backend and data model  ·  1:05

The backend is one Python process. A paho MQTT client subscribes to the six topics and writes each one to its own table: telemetry, pump events, faults, availability and commands, plus a configuration table. We chose SQLite on purpose. The lectures explain why relational engines struggle with high-rate time series, but that happens at thousands of writes per second; we write one row per second from one node, so a dedicated time series engine would add operational work for no measurable gain. We still follow the schema principle from the lectures: only the timestamp is indexed, and the sequence number, which changes on every row, is deliberately not, to avoid an index explosion. The REST API is on the right. Reads use GET because they are safe. The configuration uses PUT because replacing the threshold set is idempotent. Commands use POST because each one creates a new record. The two highlighted endpoints change the physical world, so they require an administrator session.

## 24. Dashboard and remote access  ·  1:20

This is the dashboard. It is a single HTML page with no framework. The top part is a process mimic in the style of a control room: the source bucket, the pump, the pipe and the tank, with the water drawn at its measured height against a ruler, and the thirty, seventy and overflow lines at their true positions, so anyone can see why the pump just switched. Below are the tiles for level, flow, volume and pump current with the health label, and next to them the level history. A freshness indicator shows how many seconds old the data is. For this screenshot, we replayed recorded bench data into an isolated copy of the system, so the real database was not touched. The dashboard is public: a Cloudflare tunnel gives it an HTTPS address, so it can be watched from any phone, without opening a port on our network. Control is private: the buttons stay hidden until the browser has an administrator session. We first tried to allow control only from the host machine's address, and rejected it: behind the tunnel every request arrives from 127.0.0.1, so that rule would have given control to the whole Internet.

## 25. Security model  ·  1:20

The ITU model in the lectures treats security as a capability that cuts across every layer, so we analysed threats layer by layer. At the broker, anonymous access is disabled, and an access control list follows least privilege: the device account cannot publish commands, so even a compromised ESP32 cannot be used to drive the pump. At the API, every endpoint that changes the physical world requires a session. The password lives in a file outside the repository and is compared in constant time; a correct password produces a random 256-bit token in an HttpOnly, SameSite Strict, Secure cookie; and five wrong attempts in five minutes lock that address. The two blue rows are the interesting ones, because they are stopped by the control design itself. Forged telemetry cannot fake a low level into an overflow, because the float and the raw distance guard are on the device. And spamming commands cannot wear out the pump, because the minimum on and off times are enforced in firmware. Our known gaps are in the report: no TLS on the lab broker, one shared admin password, and sessions held in memory.

## 26. Behaviour during a network outage  ·  1:20

What happens when the network fails? Four mechanisms. First, the control loop never waits for the network. The lectures warn against connection logic inside a blocking loop. Our reconnection is non-blocking, but opening a TCP socket to an unreachable broker still blocks inside the network stack, three seconds by default on the ESP32. We cap it at half a second, and the device publishes the longest loop period in the first message after reconnecting, so the claim can be checked rather than assumed. Second, nothing is forgotten: while offline, telemetry goes into a ring buffer of 240 samples, four minutes at one hertz, which is replayed in order when the link returns, marked as replay so it is excluded from latency statistics. Accumulated volume is saved to flash every minute. Third, everyone knows: the broker publishes the last will after the keep-alive expires, and the dashboard independently turns red after five seconds without data. Fourth, the device finds its way back, with exponential backoff, up to three Wi-Fi networks, and the broker located again by name through mDNS, because a laptop gets a new address on each network.

## 27. Experiments and results  ·  0:10

Part six: experiments and results.

## 28. Experiment plan  ·  0:55

Our experimental rule was simple: every measurement needs a reference that is independent of the system being measured, and every number must come from the database by a script that anyone can rerun. The scripts are in the repository: analyze_experiments for cycles, switching and latency, and bench for calibration, fault injection with a timestamped key press, and outage tests. This table lists the experiments, the independent reference for each, and the status. The main session we analyse is September 23rd, an hour and twenty-four minutes of operation on the final filter configuration: 4870 messages, 99.5 percent of them with a trusted level, and not a single fault raised. For the level, we checked repeatability against two fixed physical references: still water, and the lower float switch, a mechanical contact at a fixed height. Over eight closings, the float was seen at 4.55 centimetres with a standard deviation of only 0.074, far smaller than our control band. The absolute check with a ruler is the one step we still run before the defence.

## 29. Control response and switching  ·  1:30

This is the level over seventy-six minutes on the bench. Blue bands are the pump running; hatched areas are manual mode, which we used for command tests and to drain the tank below the band on purpose. In automatic mode, every fill stopped between 70.1 and 70.8 percent against a seventy percent threshold. The level then rises a little more, 1.4 percent on average and 1.85 at most, because water still in the pipe keeps arriving after the relay opens. The highest peak, 72.4 percent, leaves more than twelve points of margin to the overflow line in orange. A fill from the bottom of the band takes about two and a half minutes. Over the session the pump started 12.9 times per hour, including manual starts, and the shortest rest between a stop and the next start was 32 seconds, above our twenty second minimum, so the relay never chattered. And not one fault rule fired falsely. One surprise: automatic fills started at 32 to 33 percent, not 30. In every case the low float engaged in that same second. The float is mounted slightly above the thirty percent line, and because either path may start the pump, the float wins. The effective threshold is set by a screw on the tank wall.

## 30. Volume estimation error  ·  1:10

How accurate is the accumulated volume? For each automatic fill we compare the device's pumped volume with a reference built only from the level and the tank geometry: the rise in level times the base area, plus what drained out during the fill. The drain rate is measured independently, from the slope of the level with the pump off, ninety seconds before and after the fill, and averaged, because by Torricelli's law the drain is faster when the tank is fuller. Over six complete fills, the mean error is minus 5.2 percent, ranging from minus 15 to plus 8. The pattern is informative: the three largest negative errors coincide with the three largest drain rates. That is what a constant pump delivery predicts, since the real pump output varies with the water level in the source bucket while our model keeps 0.36. So this error belongs to treating delivery as a constant, and the remedy is a working inlet flow sensor, which is our main open item.

## 31. Command latency  ·  1:20

For latency, our headline number is the command round trip, not the one-way telemetry delay, and the reason is methodological. A one-way delay subtracts two clocks, the server's and the ESP32's, and network time on a microcontroller is only accurate to tens of milliseconds, the same order as what we measure. Once, right after a reboot, the one-way figure read 1100 milliseconds while the round trip at that moment was 187. The round trip is stamped by the server at both ends, so the skew cancels. The chart shows how the round trip improved. Our first firmware had a median of 226 milliseconds. The big step was the radio: the ESP32 Arduino core enables modem sleep by default, so the receiver sleeps between beacons and packets wait. Turning it off cut the 95th percentile from 884 to 315 milliseconds, at the cost of roughly double radio current, which is fine for a mains-powered device. The final session reached a median of 42 milliseconds and a 95th percentile of 138, over 75 commands. That session also used a different access point, so we do not attribute the last step to a single cause.

## 32. Fault detection and network outage  ·  1:25

The outage test isolates the device alone, by blocking its traffic for sixty seconds while the broker and backend stay connected to each other, because that is the failure the brief describes. Thirty-two messages were buffered and replayed afterwards, and the pump did not start on its own. Ten messages were lost, and I want to explain why rather than hide it: they were written into a socket the device still believed was open. A silently dropped TCP connection is only detected when the fifteen-second keep alive expires, so the residual gap is bounded by the keep alive, not by the buffer size. We also learned that stopping the broker is not an equivalent test: the device reconnects and replays before the backend has resubscribed, and the replay is lost. For fault injection, the sensor timeout fires forty to forty-one seconds after the last trusted message, which matches the design: the level is marked untrusted five seconds after the last good reading, and the forty-five second timeout counts from that same reading. The other rows come from the fault and command logs of three days of testing. In all eighteen overflow and no-progress events where the pump was running, the pump-off message went out in the same two-hundred-millisecond cycle as the fault. And every manual start attempted while a fault was active was refused with the right reason, acknowledged in thirty to forty milliseconds. Two of these faults you will see live in the demo.

## 33. The result to remember: 70.4 %  ·  0:25

If you remember one number from our results, make it this one. Across every automatic fill in the analysed session, the pump stopped at 70.4 percent on average, against a threshold of seventy, with no false alarm from any of the eleven fault rules. That is what a filtered measurement, a hysteresis band and timing constraints buy you on a noisy real sensor.

## 34. Lessons and conclusion  ·  0:10

Part seven: lessons, limitations, the conclusion, and then the demo.

## 35. Bugs found on the bench  ·  1:20

Four bugs taught us the most. Each was invisible in the code and was found by counting something. First, an inverted relay: the configuration assumed the module was active low, it was active high, so every stop command started the pump while the telemetry said "off". We proved the real state from the ripple of the motor current, twenty-seven millivolts when running versus four and a half when stopped. Second, the newest one: the pump started after every reboot. At startup the firmware set the last-valid-reading timestamp to "now" so the sensor timeout counts from power-up, but the validity flag was derived from that same timestamp, so for a few seconds the level was "valid" at zero percent and the controller saw an empty tank. We found it because automatic starts at 38 to 51 percent each lined up with the sequence number restarting at one. Third, the fragile statistic in the level filter, which I mentioned earlier. Fourth, silent truncation: a telemetry message outgrew its buffer, the JSON library cut it without an error, and the MQTT library swallowed the parse exception. Everything looked alive, and nothing was stored. Both ends now report it loudly.

## 36. Limitations and next steps  ·  1:20

We think pointing precisely at what falls short is as valuable as showing what works. The main open item is flow measurement, highlighted. The cause is known and the fix is cheap: a real pulse at our flow lasts about fourteen milliseconds, while the conducted interference is far shorter, so a minimum pulse width filter would separate them, and a supply that does not share a return path with the motor would remove the cause. A sensor rated down to 0.05 litres per minute would also measure our outlet. Second, the current scale factor: we read 360 to 425 milliamps for a pump rated 100 to 200, so either the divider or the sensor variant differs from our assumption; one multimeter reading settles it, and the present-or-absent use of current is unaffected. Third, the ultrasonic beam is wider than the tank. Fourth, the sensor sometimes locks up until its power is cycled; a GPIO-controlled supply would let the firmware do that by itself. The rolling baseline needs several days to converge. And for a real deployment: a named tunnel instead of a link that changes every restart, per-user accounts, TLS on the broker, and over-the-air updates.

## 37. Conclusion  ·  1:10

To conclude, three ideas. First, we put the whole control law and every safety rule on the ESP32, following the rule that anything needing an immediate safe stop belongs at the edge; as a result, the outage requirement is a property of the architecture rather than an added feature. Second, safety is structural: every path that can start the pump goes through one guard function, and overflow is prevented by independent layers, several of which do not depend on the filtered level at all. Third, we trusted measurements over datasheets and over our own expectations: the pump delivers a fifth of its rated flow, the flow sensors counted interference instead of water, and the analysis of our own data found a bug that started the pump after every reboot. On the bench, the controller stopped every automatic fill between 70.1 and 70.8 percent, raised no false alarm, estimated volume within minus five percent on average, and confirmed commands in 42 milliseconds. Now let us show you the real tank.

## 38. Live demonstration  ·  1:20

Now the live demo, six steps on the real tank. One: we open the drain valve and you will see the level fall, both on the projector and on a phone connected over mobile data, to show remote access. Two: when the level drops below the band, the pump starts by itself, and it stops at seventy percent. Three: we send a manual command from the dashboard; watch the screen say "sending" and then show the device's confirmed answer. Four: with the pump on manually, we lift the upper float. The pump stops immediately, an overflow fault is raised, and clearing the fault is refused while the float is still up. Five: we lift the pump intake out of the water and watch the pump health change to no_flow. Six: we switch off the Wi-Fi hotspot while the pump is filling. The dashboard turns red, but the pump still stops at seventy percent on its own; when we switch the hotspot back on, the buffered data fills the gap in the chart. If anything goes wrong with the hardware, we have a backup video of the same sequence.

## 39. Thank you  ·  0:15

Thank you for your attention. We are happy to take your questions. The full source code, the report, the code guide for newcomers and the test procedures are in the repository on screen.
## Appendix: likely questions and short answers

**Why a relay and not a MOSFET?** The pump has two states and switches a few times per hour, so the MOSFET's switching speed brings nothing; the relay module drives the load from its own supply. A 1N4007 flyback diode protects it.

**Why SQLite and not a time series database?** We write one row per second from one node, three orders of magnitude below where relational engines struggle. We still follow the time series schema rule: only the timestamp is indexed.

**Why polling and not WebSocket?** The device publishes at 1 Hz, so polling at 1 Hz wastes nothing, and a WebSocket adds a stateful channel with its own failure modes. Beyond about ten nodes or 5 Hz we would switch.

**Why is flow derived from the level if the brief requires a flow sensor?** Both YF-S401 sensors are fitted and read, but we measured about 1500 Hz of conducted interference while the pump runs, against 35 Hz of real signal, and our outlet flow is below the sensor's 0.3 L/min minimum. We chose not to control on numbers we know are wrong, and we state it in the report.

**What if the ESP32 itself hangs?** The relay is driven off as the first instruction after any reset. A watchdog reset therefore stops the pump. A hardware float cut-off wired in series with the pump would be the next step for a real installation.

**How do you know the outage test proves local control?** The replayed samples carry the pump state and sequence numbers, so the pump stopping at 70 % appears in the replayed history, and the first message after reconnection reports the longest control period during the outage.

**Why is the sensor timeout 45 seconds and not shorter?** The ultrasonic sensor drops out for tens of seconds during normal fills; with 4 seconds every fill stopped early. During that window the float, the raw distance guard and the run limits still protect the tank.
