#!/bin/sh

export SERVICE_PATH="/etc/led_controller"
export SCRIPT_NAME=$(basename "$0")
export LOG_FILE="/etc/led_controller/setting_daemon.log"

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Removing all led_controller files from system ..."  >> $LOG_FILE
/etc/init.d/led-settings-daemon stop
rm /etc/rc.d/S98led-settings-daemon
rm /etc/rc.d/K99led-settings-daemon
rm /etc/init.d/led-settings-daemon
rm -rf $SERVICE_PATH
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> $LOG_FILE