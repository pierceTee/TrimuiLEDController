#!/bin/sh
BASE_LED_PATH="/sys/class/led_anim"
TOP_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale"
FRONT_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_f1f2"
BACK_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_lr"
SCRIPT_NAME=$(basename "$0")

chmod a+w $BASE_LED_PATH/*
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Turning on all LEDs"
echo 200 | tee $TOP_BRIGHTNESS_PATH $FRONT_LED_BRIGHTNESS_PATH $BACK_LED_BRIGHTNESS_PATH