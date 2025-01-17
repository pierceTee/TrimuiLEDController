#!/bin/sh
BASE_LED_PATH="/sys/class/led_anim"
SERVICE_PATH="/etc/led_controller"
SCRIPT_NAME=$(basename "$0")

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Removing all led_controller files from system ..."
# Stop our service
/etc/init.d/led-settings-daemon stop

# Remove our service files.
rm /etc/rc.d/*led-settings-daemon
rm /etc/init.d/led-settings-daemon
rm -rf $SERVICE_PATH


# Restore the stock low battery LED script
if [ -f "/usr/trimui/bin/low_battery_led.sh.bak" ]; then
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Restoring stock low battery LED script ..."
  rm -f /usr/trimui/bin/low_battery_led.sh
  mv /usr/trimui/bin/low_battery_led.sh.bak /usr/trimui/bin/low_battery_led.sh
fi

# Re-enabled led file access
chmod a+w $BASE_LED_PATH/*
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."