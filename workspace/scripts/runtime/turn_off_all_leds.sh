#!/bin/sh
BASE_LED_PATH="/sys/class/led_anim"
TOP_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale"
FRONT_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_f1f2"
BACK_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_lr"
SCRIPT_NAME=$(basename "$0")

chmod a+w $BASE_LED_PATH/*
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Turning off all LEDs"
# Turn brightness off for all LEDs
echo 0 | tee $TOP_BRIGHTNESS_PATH $FRONT_LED_BRIGHTNESS_PATH $BACK_LED_BRIGHTNESS_PATH

# Change all LEDs to static (I think the OS looks for changes to the effect files 
# to know when to update the LEDs)
echo 0 | tee $BASE_LED_PATH/effect_lr $BASE_LED_PATH/effect_m $BASE_LED_PATH/effect_f1 $BASE_LED_PATH/effect_f2
