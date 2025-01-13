#!/bin/sh

export SERVICE_PATH="/etc/led_controller"
export SERVICE_NAME="led-settings-daemon"
export LOG_FILE="led_controller.log"

SCRIPT_NAME=$(basename "$0")

# Write settings to system path.
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Writing LED information to configuration files ..."  >> $LOG_FILE

mkdir -p $SERVICE_PATH
cp settings.ini settings-daemon.sh $SERVICE_PATH
cp led-settings-daemon /etc/init.d/
chmod +x /etc/init.d/led-settings-daemon

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Finished writing service information to  $SERVICE_PATH ..."  >> $LOG_FILE

# Manually create symlinks in /etc/rc.d

#Start the service with a priority of 97.
ln -s /etc/init.d/led-settings-daemon /etc/rc.d/S99led-settings-daemon

#Stop the service with a priority of 03.
ln -s /etc/init.d/led-settings-daemon /etc/rc.d/K03led-settings-daemon

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Starting  $SERVICE_NAME ..."  >> $LOG_FILE
/etc/init.d/led-settings-daemon start

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> $LOG_FILE