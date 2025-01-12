export SERVICE_PATH="/etc/led_controller"
export SERVICE_NAME="led-settings-daemon.service"
SCRIPT_NAME=$(basename "$0")

# Write settings to system path.
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Writing LED information to configuration files ..."  >> system_io.log

mkdir -p $SERVICE_PATH
cp settings.ini > $SERVICE_PATH
cp led-settings-daemon.service $SERVICE_PATH
chmod +x $SERVICE_PATH/$SERVICE_NAME

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Finished writing service information to  $SERVICE_PATH ..."  >> system_io.log

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Starting  $SERVICE_NAME ..."  >> system_io.log
sudo systemctl enable led-settings-daemon.service
sudo systemctl start led-settings-daemon.service

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> system_io.log