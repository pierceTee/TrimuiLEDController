#!/bin/sh
export BASE_LED_PATH="/sys/class/led_anim"
export GENERAL_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale"
export FRONT_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_f1f2"
export BACK_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_lr"
export SERVICE_PATH="/etc/led_controller"
export SERVICE_NAME="led-settings-daemon"
export SCRIPT_NAME=$(basename "$0")
export LOG_FILE="led_controller.log"


if [ "$1" = "install" ]; then
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Writing LED information to configuration files ..."  >> $LOG_FILE

mkdir -p $SERVICE_PATH
cp settings.ini settings-daemon.sh $SERVICE_PATH
cp led-settings-daemon /etc/init.d/
chmod +x /etc/init.d/led-settings-daemon

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Finished writing service information to  $SERVICE_PATH ..."  >> $LOG_FILE

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> $LOG_FILE

elif [ "$1" = "uninstall" ]; then
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Removing all led_controller files from system ..."  >> $LOG_FILEdoes 
  /etc/init.d/led-settings-daemon stop
  rm /etc/rc.d/*led-settings-daemon
  rm /etc/init.d/led-settings-daemon
  rm -rf $SERVICE_PATH
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."  >> $LOG_FILE

elif [ "$1" = "on" ]; then
  chmod a+w $BASE_LED_PATH/* >> $LOG_FILE
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Turning on all LEDs"  >> $LOG_FILE
  echo 200 | tee $GENERAL_BRIGHTNESS_PATH $FRONT_LED_BRIGHTNESS_PATH $BACK_LED_BRIGHTNESS_PATH >>   $LOG_FILE

elif [ "$1" = "off" ]; then
  chmod a-w $BASE_LED_PATH/* >> $LOG_FILE
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Turning off all LEDs"  >> $LOG_FILE
  echo 0 | tee $GENERAL_BRIGHTNESS_PATH $FRONT_LED_BRIGHTNESS_PATH $BACK_LED_BRIGHTNESS_PATH >>   $LOG_FILE

else
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Invalid parameter. Usage: $SCRIPT_NAME [on|off|intall|uninstall]" >> $LOG_FILE
fi

