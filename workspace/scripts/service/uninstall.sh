export SERVICE_PATH="/etc/led_controller"
export SCRIPT_NAME=$(basename "$0")

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Removing all led_controller files from system ..."  >> io_log.log
rf -rf $SERVICE_PATH > io_log.log
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> io_log.log