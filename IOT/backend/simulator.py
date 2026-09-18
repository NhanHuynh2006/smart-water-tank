"""
Mo phong nut bien: mo hinh thuy luc + may trang thai + phat MQTT dung dac ta.

Hai cach dung:
  python simulator.py            chay nhu mot ESP32 ao, phat 1 Hz len broker
  python simulator.py --sweep    quet tham so dieu khien, khong can broker

Cac che do loi co the tiem:
  --fault dryrun    bom chay ma khong len nuoc
  --fault stuck     cam bien muc ket gia tri
  --fault leak      ro ri 0.3 L/phut khi bom tat
"""
import argparse
import json
import math
import random
import time
import uuid

G = 981.0          # cm/s^2
AREA = 400.0       # cm^2, tiet dien bon
MAX_LEVEL = 25.0   # cm
ORIFICE = 0.09     # cm^2, lo thao (chon sao cho Qout < Qbom)
CD = 0.62          # he so luu luong
PUMP_LPM = 1.5     # luu luong bom


class Tank:
    """Mo hinh dong hoc: dV/dt = Qin - Qout - Qleak, Qout theo Torricelli."""

    def __init__(self, level_cm=12.0):
        self.h = level_cm
        self.volume_total = 0.0

    def step(self, dt, pump_on, drain_open, leak_lpm=0.0, dry=False):
        q_in = PUMP_LPM if (pump_on and not dry) else 0.0
        # Torricelli: Q = Cd * Ao * sqrt(2 g h)  -> cm^3/s -> L/phut
        q_out = 0.0
        if drain_open and self.h > 0:
            q_out = CD * ORIFICE * math.sqrt(2 * G * self.h) * 60.0 / 1000.0
        q_net = q_in - q_out - leak_lpm
        dh = (q_net * 1000.0 / 60.0) * dt / AREA
        self.h = max(0.0, min(MAX_LEVEL, self.h + dh))
        self.volume_total += q_in * dt / 60.0
        return q_in, q_out


class Controller:
    def __init__(self, low=30.0, high=80.0, min_on=10.0, min_off=20.0):
        self.low, self.high = low, high
        self.min_on, self.min_off = min_on, min_off
        self.pump = False
        self.t_on = self.t_off = -1e9
        self.switches = 0

    def update(self, t, level_pct):
        if self.pump:
            if level_pct > self.high and (t - self.t_on) >= self.min_on:
                self.pump = False
                self.t_off = t
                self.switches += 1
        else:
            if level_pct < self.low and (t - self.t_off) >= self.min_off:
                self.pump = True
                self.t_on = t
                self.switches += 1
        return self.pump


def run_sweep():
    """Quet tham so tren mo hinh, so sanh nguong don voi vung tre."""
    configs = []
    for lo, hi in [(49, 51), (45, 55), (40, 60), (30, 80), (20, 90)]:
        for timing in (False, True):
            configs.append((lo, hi, timing))

    print(f"{'nguong':>12} {'rang buoc TG':>14} {'dong cat/gio':>14} "
          f"{'bom chay (s)':>14} {'ngoai dai (s)':>14}")
    print("-" * 72)
    results = []
    for lo, hi, timing in configs:
        tank = Tank(50.0 * MAX_LEVEL / 100)
        ctl = Controller(lo, hi,
                         min_on=10.0 if timing else 0.0,
                         min_off=20.0 if timing else 0.0)
        dt, T = 0.2, 1800.0
        t = 0.0
        pump_seconds = 0.0
        out_of_band = 0.0
        while t < T:
            pct = tank.h / MAX_LEVEL * 100
            noise = random.gauss(0, 0.4)          # nhieu do
            pump = ctl.update(t, max(0.0, min(100.0, pct + noise)))
            tank.step(dt, pump, drain_open=True)
            if pump:
                pump_seconds += dt
            if not (25.0 <= pct <= 85.0):          # dai muc tieu co dinh
                out_of_band += dt
            t += dt
        per_hour = ctl.switches / (T / 3600.0)
        results.append((lo, hi, timing, per_hour, pump_seconds, out_of_band))
        print(f"{f'{lo}/{hi}%':>12} {'co' if timing else 'khong':>14} "
              f"{per_hour:>14.1f} {pump_seconds:>14.1f} {out_of_band:>14.1f}")

    base = next(r for r in results if r[0] == 49 and not r[2])
    best = next(r for r in results if r[0] == 30 and r[2])
    if base[3] > 0:
        red = (base[3] - best[3]) / base[3] * 100
        print("-" * 72)
        print(f"Nguong don 49/51 khong rang buoc : {base[3]:.1f} lan dong cat moi gio")
        print(f"Vung tre 30/80 co rang buoc      : {best[3]:.1f} lan dong cat moi gio")
        print(f"Ti le giam eta                   : {red:.1f} %")


def run_live(args):
    import paho.mqtt.client as mqtt

    root = f"wt/{args.site}/{args.dev}"
    cli = mqtt.Client(client_id=f"sim-{uuid.uuid4().hex[:6]}")
    if args.user:
        cli.username_pw_set(args.user, args.password)
    cli.will_set(f"{root}/status", json.dumps({"online": False}), qos=1, retain=True)

    tank = Tank(12.0)
    ctl = Controller()
    mode = {"auto": True}
    manual_pump = {"on": False}
    fault = {"code": ""}
    seq = {"n": 0}
    vol_out = {"l": 0.0}      # the tich da chay ra, cho cam bien dau ra

    def send_ack(cid, status, reason=None):
        p = {"dev": args.dev, "cmd_id": cid, "status": status,
             "state": "FILLING" if ctl.pump else "IDLE",
             "pump": ctl.pump, "mode": "AUTO" if mode["auto"] else "MANUAL",
             "ts": int(time.time())}
        if reason:
            p["reason"] = reason
        cli.publish(f"{root}/cmd/ack", json.dumps(p), qos=1)

    def block_reason():
        pct = tank.h / MAX_LEVEL * 100
        if fault["code"]:
            return "fault_active"
        if pct >= 95:
            return "overflow_guard"
        return None

    def on_msg(c, u, msg):
        d = json.loads(msg.payload.decode())
        cid, act = d.get("cmd_id", "?"), d.get("action")
        if act == "mode":
            mode["auto"] = bool(d.get("value"))
            send_ack(cid, "accepted")
        elif act == "pump":
            if mode["auto"]:
                send_ack(cid, "rejected", "auto_mode_active")
                return
            want = bool(d.get("value"))
            if want:
                why = block_reason()
                if why:
                    send_ack(cid, "rejected", why)
                    return
            manual_pump["on"] = want
            send_ack(cid, "accepted")
        elif act == "clear_fault":
            if not fault["code"]:
                send_ack(cid, "rejected", "no_active_fault")
            else:
                fault["code"] = ""
                send_ack(cid, "accepted")
        elif act == "reset_volume":
            send_ack(cid, "accepted")
        else:
            send_ack(cid, "rejected", "unknown_action")

    cli.on_message = on_msg
    cli.on_connect = lambda c, u, f, rc: (
        c.publish(f"{root}/status", json.dumps({"online": True}), qos=1, retain=True),
        c.subscribe(f"{root}/cmd", qos=1),
        print(f"[SIM] ket noi broker rc={rc}"))
    cli.connect(args.host, args.port, keepalive=30)
    cli.loop_start()

    dt = 0.2
    t = 0.0
    last_pub = 0.0
    last_pump = None
    dry_since = None
    print(f"[SIM] dang phat len {root}/telemetry, Ctrl+C de dung")

    try:
        while True:
            pct_true = tank.h / MAX_LEVEL * 100
            pct_meas = pct_true if args.fault != "stuck" else 55.0
            pct_meas = max(0.0, min(100.0, pct_meas + random.gauss(0, 0.4)))

            if mode["auto"]:
                pump = ctl.update(t, pct_meas) if not fault["code"] else False
            else:
                pump = manual_pump["on"] and not fault["code"]
                if pump != (last_pump or False):
                    ctl.switches += 1

            leak_lpm = 0.3 if (args.fault == "leak" and not pump) else 0.0
            dry = (args.fault == "dryrun")
            q_in, q_out = tank.step(dt, pump, drain_open=args.drain,
                                    leak_lpm=leak_lpm, dry=dry)
            vol_out["l"] += q_out * dt / 60.0    # cam bien luu luong DAU RA

            # Luat chay kho chay ngay tren nut bien
            if pump and q_in < 0.25:
                dry_since = dry_since or t
                if t - dry_since > 6.0 and not fault["code"]:
                    fault["code"] = "DRY_RUN"
                    cli.publish(f"{root}/event/fault", json.dumps(
                        {"dev": args.dev, "code": "DRY_RUN", "ts": int(time.time()),
                         "level_pct": round(pct_meas, 1), "flow_lpm": 0.0}), qos=1)
                    print("[SIM] phat hien DRY_RUN")
            else:
                dry_since = None

            if pump != last_pump:
                last_pump = pump
                cli.publish(f"{root}/state/pump", json.dumps(
                    {"dev": args.dev, "pump": pump,
                     "state": "FILLING" if pump else "IDLE",
                     "ts": int(time.time())}), qos=1, retain=True)

            if t - last_pub >= 1.0:
                last_pub = t
                seq["n"] += 1
                payload = {
                    "dev": args.dev, "ts": int(time.time()),
            "ts_ms": int(time.time() * 1000), "seq": seq["n"],
                    "level_pct": round(pct_meas, 1),
                    "level_cm": round(pct_meas * MAX_LEVEL / 100, 2),
                    "level_ok": args.fault != "stuck",
                    "flow_lpm": round(q_in + leak_lpm, 2),
                    "volume_l": round(tank.volume_total, 2),
                    "volume_today_l": round(tank.volume_total, 2),
                    "flow_out_lpm": round(q_out, 2),
                    "volume_out_l": round(vol_out["l"], 2),
                    "volume_out_today_l": round(vol_out["l"], 2),
                    "pump": pump,
                    "state": ("FAULT_DRYRUN" if fault["code"]
                              else ("FILLING" if pump else "IDLE")),
                    "mode": "AUTO" if mode["auto"] else "MANUAL",
                    "current_mv": 38.0 if pump else 0.5,
                    "float_max": pct_true >= 99, "float_src": True,
                    "fault": fault["code"], "rssi": -55}
                cli.publish(f"{root}/telemetry", json.dumps(payload), qos=0)

            t += dt
            time.sleep(dt / args.speed)
    except KeyboardInterrupt:
        cli.publish(f"{root}/status", json.dumps({"online": False}),
                    qos=1, retain=True)
        cli.loop_stop()
        print("\n[SIM] da dung")


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--sweep", action="store_true", help="quet tham so, khong can broker")
    ap.add_argument("--host", default="localhost")
    ap.add_argument("--port", type=int, default=1883)
    ap.add_argument("--user", default="device1")
    ap.add_argument("--password", default="device_pass")
    ap.add_argument("--site", default="lab1")
    ap.add_argument("--dev", default="esp32-01")
    ap.add_argument("--drain", action="store_true", default=True,
                    help="mo van xa lien tuc")
    ap.add_argument("--fault", choices=["none", "dryrun", "stuck", "leak"],
                    default="none")
    ap.add_argument("--speed", type=float, default=1.0,
                    help="he so tang toc thoi gian mo phong")
    a = ap.parse_args()
    if a.sweep:
        run_sweep()
    else:
        run_live(a)
