#!/usr/bin/env bash
# ============================================================
#  Bat toan bo he thong bon nuoc va mo dashboard.
#
#    ./start.sh            bat broker + backend, mo trinh duyet
#    ./start.sh sim        bat them ESP32 ao (khi chua cam mach that)
#    ./start.sh sim 30     ESP32 ao chay nhanh 30 lan
#    ./start.sh public     dua dashboard ra Internet qua Cloudflare Tunnel
#    ./stop.sh             tat tat ca
# ============================================================
set -u
cd "$(dirname "$0")/IOT" || exit 1

export PATH="$HOME/.local/bin:$PATH"

# ---------- Che do cong khai ----------
# Dashboard co nut BAT BOM. Dua no ra Internet nghia la ai co duong link
# cung bat duoc bom that. Vi vay che do nay LUON sinh mot ma dieu khien va
# bat backend doi ma do o moi lenh. Xem thi tu do, dieu khien thi phai co ma.
if [ "${1:-}" = "public" ]; then
  TOKFILE="$HOME/.cache/water-tank/token"
  mkdir -p "$(dirname "$TOKFILE")"
  [ -s "$TOKFILE" ] || head -c 18 /dev/urandom | base64 | tr -d "/+=" > "$TOKFILE"
  export WT_TOKEN="$(cat "$TOKFILE")"
  echo "  ma dieu khien: $WT_TOKEN"
  echo "                 (trang se hoi ma nay khi ban bam nut bat tat bom)"
  if ! command -v cloudflared > /dev/null; then
    echo
    echo "  CHUA CO cloudflared. Cai mot lan bang:"
    echo "    curl -L -o ~/.local/bin/cloudflared \\"
    echo "      https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64"
    echo "    chmod +x ~/.local/bin/cloudflared"
    echo
    echo "  Chua co thi van chay duoc trong cung Wi-Fi qua dia chi IP ben duoi."
  fi
fi
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
IP="$(hostname -I 2>/dev/null | awk "{print \$1}")"
[ -n "$IP" ] && echo "  Trong Wi-Fi : http://$IP:8000/"
if [ "${1:-}" = "public" ] && command -v cloudflared > /dev/null; then
  echo "  Dang mo duong ra Internet, cho dia chi ben duoi..."
  nohup cloudflared tunnel --url http://localhost:8000 > "$LOG/tunnel.log" 2>&1 &
  for _ in $(seq 40); do
    URL="$(grep -o "https://[a-z0-9-]*\.trycloudflare\.com" "$LOG/tunnel.log" 2>/dev/null | head -1)"
    [ -n "$URL" ] && break
    sleep 0.5
  done
  [ -n "$URL" ] && echo "  Cong khai   : $URL" \
                || echo "  Cong khai   : chua mo duoc, xem $LOG/tunnel.log"
fi
echo "  Tai lieu API: http://localhost:8000/docs"
command -v xdg-open > /dev/null && nohup xdg-open http://localhost:8000/ > /dev/null 2>&1 &
exit 0
