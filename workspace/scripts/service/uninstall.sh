export SERVICE_PATH="/etc/led_controller"
export SCRIPT_NAME=$(basename "$0")
export LOG_FILE="led_controller.log"

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Removing all led_controller files from system ..."  >> $LOG_FILE
rf -rf $SERVICE_PATH > $LOG_FILE
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> $LOG_FILE