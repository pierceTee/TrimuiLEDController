#!/bin/bash

export SETTINGS_FILE="/etc/led_controller/settings.ini"
export SYS_FILE_PATH="/sys/class/led_anim"
export SERVICE_PATH="/etc/led_controller"
export BASE_LED_PATH="/sys/class/led_anim"
export SCRIPT_NAME=$(basename "$0")

# Function to apply settings for a specific LED
apply_led_settings() {
    local led=$1
    local brightness=$2
    local color=$3
    local duration=$4
    local effect=$5

    echo "[$SCRIPT_NAME]: Writing $led LED information to configuration files ..."
    if [[ $led == "f1f2" ]]; then
        echo $brightness > "$SYS_FILE_PATH/max_scale_f1"
        echo $color > "$SYS_FILE_PATH/effect_rgb_hex_f2"
        echo $duration > "$SYS_FILE_PATH/effect_duration_f1"
        echo $effect > "$SYS_FILE_PATH/effect_f1"
    else
        echo $brightness > "$SYS_FILE_PATH/max_scale_$led"
        echo $color > "$SYS_FILE_PATH/effect_rgb_hex_$led"
        echo $duration > "$SYS_FILE_PATH/effect_duration_$led"
        echo $effect > "$SYS_FILE_PATH/effect_$led"
    fi
}

# == Start of the script ==
echo "[$SCRIPT_NAME]: LED settings daemon started ..."

# Enable write permissions on LED files
chmod -v a+w $LED_PATH/* 

echo "[$SCRIPT_NAME]: Writing LED information to configuration files ..."
# Read settings from the settings file and apply them
while IFS= read -r line; do
    if [[ $line =~ ^\[([a-zA-Z0-9]+)\]$ ]]; then
        led=${BASH_REMATCH[1]}
    elif [[ $line =~ ^brightness=([0-9]+)$ ]]; then
        brightness=${BASH_REMATCH[1]}
    elif [[ $line =~ ^color=0x([0-9A-Fa-f]+)$ ]]; then
        color=${BASH_REMATCH[1]}
    elif [[ $line =~ ^duration=([0-9]+)$ ]]; then
        duration=${BASH_REMATCH[1]}
    elif [[ $line =~ ^effect=([0-9]+)$ ]]; then
        effect=${BASH_REMATCH[1]}
        apply_led_settings $led $brightness $color $duration $effect
    fi
done < "$SETTINGS_FILE"

# Disable write permissions on LED files
chmod -v a-w $LED_PATH/*

echo "[$SCRIPT_NAME]: Finished writing LED information to configuration files ..."
echo "[$SCRIPT_NAME]: LED settings daemon stopped ..."
echo "[$SCRIPT_NAME]: exiting ..."