#!/bin/sh
# Becomes LedController.pak/launch.sh
export BASE_LED_PATH="/sys/class/led_anim"
export LED_PATH="$BASE_LED_PATH/max_scale"
export FRONT_LED_PATH="$BASE_LED_PATH/max_scale_f1f2"
export BACK_LED_PATH="$BASE_LED_PATH/max_scale_lr"
export LOG_FILE="led_controller.log"

echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Launching cd $(dirname "$0")/led_controller" >> $LOG_FILE

# Allow the application to change the LEDs
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Enabling write permissions on $LED_PATH files"  >> $LOG_FILE
chmod -v a+w $LED_PATH/* >> $LOG_FILE

# turn LEDS on
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Turning on all LEDs"  >> $LOG_FILE
echo 200 | tee $LED_PATH $FRONT_LED_PATH $BACK_LED_PATH >> $LOG_FILE


echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Launching led_controller..."  >> $LOG_FILE
./led_controller > led_controller.log 2>&1

# Prevent other applications from changing the LEDs
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Disabling write permissions on $LED_PATH files"  >> $LOG_FILE
chmod -v a-w $LED_PATH/* >> $LOG_FILE

echo "[`date '+%Y-%m-%d %H:%M:%S'`]: exiting..."  >> $LOG_FILE
