export SERVICE_PATH="/etc/led_controller"
export SERVICE_NAME="led-settings-daemon.service"
export LOG_FILE="led_controller.log"

SCRIPT_NAME=$(basename "$0")

# Write settings to system path.
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Writing LED information to configuration files ..."  >> $LOG_FILE

mkdir -p $SERVICE_PATH
cp settings.ini led-settings-daemon.service settings-daemon.sh $SERVICE_PATH
chmod +x $SERVICE_PATH/$SERVICE_NAME

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Finished writing service information to  $SERVICE_PATH ..."  >> $LOG_FILE

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Starting  $SERVICE_NAME ..."  >> $LOG_FILE
systemctl enable led-settings-daemon.service
systemctl start led-settings-daemon.service

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> $LOG_FILE