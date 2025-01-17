#!/bin/sh

SYS_SERVICE_PATH="/etc/led_controller"
SERVICE_NAME="led-settings-daemon"

SCRIPT_NAME=$(basename "$0")

# Write settings to system path.
echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Writing LED information to configuration files ..."
mkdir -p $SYS_SERVICE_PATH
cp settings.ini service/settings-daemon.sh $SYS_SERVICE_PATH
cp service/led-settings-daemon /etc/init.d/
chmod +x /etc/init.d/led-settings-daemon

# Start service
/etc/init.d/led-settings-daemon enable

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Finished writing service information to  $SYS_SERVICE_PATH ..."


# Read the should_enable_low_battery_indication setting
if [ ! -f "settings.ini" ]; then
    echo "Error: settings.ini not found"
    exit 1
fi
LOW_BATTERY_ENABLED=$(grep -m 1 "should_enable_low_battery_indication" settings.ini | cut -d'=' -f2 | tr -d '[:space:]' || echo "0")

# If enabled, replace the stock script with our custom one
if [ "$LOW_BATTERY_ENABLED" = "1" ]; then

  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: copying low battery LED script to /etc/init.d/ ..."

  # Copy custom low battery LED script to service directory
  cp service/led_controller_low_battery_led.sh $SYS_SERVICE_PATH

  # Backup stock led script if not already backed up
  if [ -f "/usr/trimui/bin/low_battery_led.sh" ] && [ ! -f "/usr/trimui/bin/low_battery_led.sh.bak" ]; then

    # Copy stock OS low_battery_led.sh to service directory
    cp /usr/trimui/bin/low_battery_led.sh $SYS_SERVICE_PATH/trimui_low_battery_led.sh
    
    # Create backup
    cp /usr/trimui/bin/low_battery_led.sh /usr/trimui/bin/low_battery_led.sh.bak
  fi

  # Remove existing script if present
  rm -f /usr/trimui/bin/low_battery_led.sh

  # Symlink custom script to stock script location
  ln -sf $SYS_SERVICE_PATH/led_controller_low_battery_led.sh /usr/trimui/bin/low_battery_led.sh
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Enabled custom low battery LED script"
fi

# If disabled, restore the stock script
if [ "$LOW_BATTERY_ENABLED" = "0" ]; then
  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Restoring stock low battery LED script ..."
  
  # Remove symlink if it exists
  if [ -L "/usr/trimui/bin/low_battery_led.sh" ]; then
    rm /usr/trimui/bin/low_battery_led.sh
  fi

  # Restore backup if it exists
  if [ -f "/usr/trimui/bin/low_battery_led.sh.bak" ]; then
    mv /usr/trimui/bin/low_battery_led.sh.bak /usr/trimui/bin/low_battery_led.sh
  fi

  echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: Restored stock low battery LED script"
fi

echo "[`date '+%Y-%m-%d %H:%M:%S'`][$SCRIPT_NAME]: exiting ..."