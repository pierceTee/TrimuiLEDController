#!/bin/sh
# Becomes LedController.pak/launch.sh
export BASE_LED_PATH="/sys/class/led_anim"
export LED_PATH="$BASE_LED_PATH/max_scale"
export FRONT_LED_PATH="$BASE_LED_PATH/max_scale_f1f2"
export BACK_LED_PATH="$BASE_LED_PATH/max_scale_lr"

echo $0 $*
cd $(dirname "$0")
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Launching cd $(dirname "$0")/led_controller" >> launch.log

# Allow the application to change the LEDs
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Enabling write permissions on $LED_PATH files"  >> launch.log
chmod -v a+w $LED_PATH/* >> launch.log

# turn LEDS on
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Turning on all LEDs"  >> launch.log
echo 200 | tee $LED_PATH $FRONT_LED_PATH $BACK_LED_PATH >> launch.log


echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Launching led_controller..."  >> launch.log
./led_controller > led_controller.log 2>&1

# Prevent other applications from changing the LEDs
echo "[`date '+%Y-%m-%d %H:%M:%S'`]: Disabling write permissions on $LED_PATH files"  >> launch.log
chmod -v a-w $LED_PATH/* >> launch.log

echo "[`date '+%Y-%m-%d %H:%M:%S'`]: exiting..."  >> launch.log