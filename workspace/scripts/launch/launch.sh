#!/bin/sh
# Becomes LedController.pak/launch.sh
BASE_LED_PATH="/sys/class/led_anim"
TOP_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale"
FRONT_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_f1f2"
BACK_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_lr"
LOG_FILE="led_controller.log"
UNINSTALL_EXIT_CODE=66
EXIT_OK_CODE=0

SCRIPT_NAME=$(basename "$0")

echo $0 $*
cd $(dirname "$0")

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Launching cd $(dirname "$0")/led_controller" >> $LOG_FILE

# Allow the application to change the LEDs
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Enabling write permissions on $BASE_LED_PATH files"  >> $LOG_FILE
chmod a+w $BASE_LED_PATH/* >> $LOG_FILE

# turn LEDS on
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Turning on all LEDs"  >> $LOG_FILE
echo 200 | tee $TOP_BRIGHTNESS_PATH $FRONT_LED_BRIGHTNESS_PATH $BACK_LED_BRIGHTNESS_PATH >> $LOG_FILE

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Launching led_controller..."  >> $LOG_FILE

 # Grab the exit code that the led_controller app script will return
EXIT_CODE=$(./led_controller 2>>$LOG_FILE)

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: led_controller returned: $EXIT_CODE"  >> $LOG_FILE

# Prevent other applications from changing the LEDs
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Disabling write permissions on $BASE_LED_PATH files"  >> $LOG_FILE
chmod a-w $BASE_LED_PATH/* >> $LOG_FILE

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting..."  >> $LOG_FILE
