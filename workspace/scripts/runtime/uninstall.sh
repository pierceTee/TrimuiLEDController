#!/bin/sh

export SERVICE_PATH="/etc/led_controller"
export SCRIPT_NAME=$(basename "$0")

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Removing all led_controller files from system ..."
./turn_off_all_leds.sh
/etc/init.d/led-settings-daemon stop
rm /etc/rc.d/*led-settings-daemon
rm /etc/init.d/led-settings-daemon
rm -rf $SERVICE_PATH

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."