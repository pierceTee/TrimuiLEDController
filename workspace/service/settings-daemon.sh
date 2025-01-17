#!/bin/sh

INSTALL_DIR="/etc/led_controller"
SETTINGS_FILE="$INSTALL_DIR/settings.ini"
SYS_FILE_PATH="/sys/class/led_anim"
SERVICE_PATH="/etc/led_controller"
BASE_LED_PATH="/sys/class/led_anim"
SCRIPT_NAME=$(basename "$0")
LOG_FILE="$INSTALL_DIR/settings_daemon.log"

# Function to apply settings for a specific LED
apply_led_settings() {
    local led=$1
    local brightness=$2
    local color=$3
    local duration=$4
    local effect=$5

    echo "[$SCRIPT_NAME]: Writing $led LED information to configuration files ..." | tee -a "$LOG_FILE"
    if [ "$led" = "f1f2" ]; then
        echo $brightness > "$SYS_FILE_PATH/max_scale_$led"
        echo $color > "$SYS_FILE_PATH/effect_rgb_hex_f1"
        echo $color > "$SYS_FILE_PATH/effect_rgb_hex_f2"
        echo $duration > "$SYS_FILE_PATH/effect_duration_f1"
        echo $duration > "$SYS_FILE_PATH/effect_duration_f2"
        echo $effect > "$SYS_FILE_PATH/effect_f1"
        echo $effect > "$SYS_FILE_PATH/effect_f2"
    else
        if [ "$led" = "m" ]; then
            # Why are you like this TrimUI?
            echo $brightness > "$SYS_FILE_PATH/max_scale"
        else
            echo $brightness > "$SYS_FILE_PATH/max_scale_$led"
        fi
        echo $color > "$SYS_FILE_PATH/effect_rgb_hex_$led"
        echo $duration > "$SYS_FILE_PATH/effect_duration_$led"
        echo $effect > "$SYS_FILE_PATH/effect_$led"
    fi
}

# == Start of the script ==
echo "[$SCRIPT_NAME]: LED settings daemon started ..." | tee -a "$LOG_FILE"

# Enable write permissions on LED files
chmod a+w $BASE_LED_PATH/* | tee -a "$LOG_FILE"

echo "[$SCRIPT_NAME]: Writing LED information to configuration files ..." | tee -a "$LOG_FILE"
# Read settings from the settings file and apply them
while IFS= read -r line; do
    if echo "$line" | grep -qE '^\[([a-zA-Z0-9]+)\]$'; then
        led=$(echo "$line" | sed -n 's/^\[\([a-zA-Z0-9]\+\)\]$/\1/p')
    elif echo "$line" | grep -qE '^brightness=([0-9]+)$'; then
        brightness=$(echo "$line" | sed -n 's/^brightness=\([0-9]\+\)$/\1/p')
    elif echo "$line" | grep -qE '^color=0x([0-9A-Fa-f]+)$'; then
        color=$(echo "$line" | sed -n 's/^color=0x\([0-9A-Fa-f]\+\)$/\1/p')
    elif echo "$line" | grep -qE '^duration=([0-9]+)$'; then
        duration=$(echo "$line" | sed -n 's/^duration=\([0-9]\+\)$/\1/p')
    elif echo "$line" | grep -qE '^effect=([0-9]+)$'; then
        effect=$(echo "$line" | sed -n 's/^effect=\([0-9]\+\)$/\1/p')
        apply_led_settings $led $brightness $color $duration $effect
    fi
done < "$SETTINGS_FILE"

# Disable write permissions on LED files
chmod a-w $BASE_LED_PATH/* | tee -a "$LOG_FILE"

echo "[$SCRIPT_NAME]: Finished writing LED information to configuration files ..." | tee -a "$LOG_FILE"
echo "[$SCRIPT_NAME]: LED settings daemon stopped ..." | tee -a "$LOG_FILE"
echo "[$SCRIPT_NAME]: exiting ..." | tee -a "$LOG_FILE"