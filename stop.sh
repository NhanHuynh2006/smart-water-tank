#!/usr/bin/env bash
# Tat he thong bon nuoc.  ./stop.sh  = tat het   |   ./stop.sh sim  = chi tat ESP32 ao
set -u
kill_by() { p=$(pgrep -f "$1"); [ -n "$p" ] && { kill $p 2>/dev/null; echo "  da tat: $2"; } || echo "  khong chay: $2"; }
if [ "${1:-}" = "sim" ]; then
  kill_by "simulator\.py" "ESP32 ao"
else
  kill_by "simulator\.py" "ESP32 ao"
  kill_by "cloudflared tunnel --no-autoupdate --url" "duong ra Internet"
  kill_by "\.venv/bin/python app\.py" "backend"
  kill_by "opt/mosquitto/sbin/mosquitto" "broker"
fi
