#!/usr/bin/env bash
# ============================================================
#  Bat toan bo he thong bon nuoc va mo dashboard.
#
#    ./start.sh            bat broker + backend, mo trinh duyet
#    ./start.sh sim        bat them ESP32 ao (khi chua cam mach that)
#    ./start.sh sim 30     ESP32 ao chay nhanh 30 lan
#    ./stop.sh             tat tat ca
# ============================================================
set -u
cd "$(dirname "$0")/IOT" || exit 1

export PATH="$HOME/.local/bin:$PATH"
PY="backend/.venv/bin/python"
LOG="$HOME/.cache/water-tank"
mkdir -p "$LOG"

running() { pgrep -f "$1" > /dev/null 2>&1; }

# ---------- 1. Broker MQTT ----------
if running "opt/mosquitto/sbin/mosquitto"; then
  echo "  broker      : dang chay san"
else
  ( cd mosquitto && nohup mosquitto -c mosquitto.conf -v > "$LOG/broker.log" 2>&1 & )
  for _ in $(seq 30); do ss -ltn 2>/dev/null | grep -q ':1883' && break; sleep 0.2; done
  ss -ltn 2>/dev/null | grep -q ':1883' \
    && echo "  broker      : da bat, cong 1883" \
    || { echo "  broker      : BAT THAT BAI, xem $LOG/broker.log"; exit 1; }
fi

# ---------- 2. Backend ----------
if running "\.venv/bin/python app\.py"; then
  echo "  backend     : dang chay san"
else
  [ -x "$PY" ] || { echo "  backend     : chua co moi truong ao, chay: cd backend && python3 -m venv .venv && .venv/bin/pip install -r requirements.txt"; exit 1; }
  # env -u PYTHONPATH: go bien cua ROS ra, neu khong thu vien ROS se che thu vien venv
  ( cd backend && env -u PYTHONPATH nohup "../$PY" app.py > "$LOG/backend.log" 2>&1 & )
  for _ in $(seq 60); do curl -s -m 1 localhost:8000/api/health > /dev/null 2>&1 && break; sleep 0.3; done
  curl -s -m 2 localhost:8000/api/health > /dev/null 2>&1 \
    && echo "  backend     : da bat, cong 8000" \
    || { echo "  backend     : BAT THAT BAI, xem $LOG/backend.log"; exit 1; }
fi

# ---------- 3. ESP32 ao, chi khi duoc yeu cau ----------
if [ "${1:-}" = "sim" ]; then
  if running "simulator\.py"; then
    echo "  ESP32 ao    : dang chay san"
  else
    ( cd backend && env -u PYTHONPATH nohup "../$PY" simulator.py --speed "${2:-10}" > "$LOG/sim.log" 2>&1 & )
    sleep 1
    running "simulator\.py" \
      && echo "  ESP32 ao    : da bat, toc do ${2:-10}x" \
      || echo "  ESP32 ao    : BAT THAT BAI, xem $LOG/sim.log"
  fi
else
  running "simulator\.py" && echo "  ESP32 ao    : dang chay (dung ./stop.sh sim de tat rieng)"
fi

# ---------- 4. Trang thai thiet bi ----------
sleep 1
curl -s -m 3 localhost:8000/api/latest 2>/dev/null | python3 -c "
import sys, json
try:
    d = json.load(sys.stdin); x = d['data']
    print(f\"  thiet bi    : {'truc tuyen' if d['online'] else 'MAT KET NOI'} · muc {x['level_pct']:.1f}% · bom {'bat' if x['pump'] else 'tat'} · {x['state']}\")
except Exception:
    print('  thiet bi    : chua co du lieu (chua cam ESP32 va chua bat ESP32 ao)')
" 2>/dev/null

echo
echo "  Dashboard   : http://localhost:8000/"
echo "  Tai lieu API: http://localhost:8000/docs"
command -v xdg-open > /dev/null && nohup xdg-open http://localhost:8000/ > /dev/null 2>&1 &
exit 0
