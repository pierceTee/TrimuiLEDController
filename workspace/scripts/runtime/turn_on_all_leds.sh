#!/bin/sh
export BASE_LED_PATH="/sys/class/led_anim"
export GENERAL_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale"
export FRONT_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_f1f2"
export BACK_LED_BRIGHTNESS_PATH="$BASE_LED_PATH/max_scale_lr"
export SCRIPT_NAME=$(basename "$0")

chmod a+w $BASE_LED_PATH/*
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Turning on all LEDs"
echo 200 | tee $GENERAL_BRIGHTNESS_PATH $FRONT_LED_BRIGHTNESS_PATH $BACK_LED_BRIGHTNESS_PATH