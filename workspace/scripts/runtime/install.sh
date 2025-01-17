#!/bin/sh

SYS_SERVICE_PATH="/etc/led_controller"
SERVICE_NAME="led-settings-daemon"

SCRIPT_NAME=$(basename "$0")

# Write settings to system path.
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Writing LED information to configuration files ..."
mkdir -p $SYS_SERVICE_PATH
cp settings.ini service/settings-daemon.sh $SYS_SERVICE_PATH
cp service/led-settings-daemon /etc/init.d/
chmod +x /etc/init.d/led-settings-daemon

# Start service
/etc/init.d/led-settings-daemon enable

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Finished writing service information to  $SYS_SERVICE_PATH ..."

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."