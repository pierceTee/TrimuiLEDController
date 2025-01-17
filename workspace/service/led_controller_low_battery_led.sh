#!/bin/sh
# My custom version of trimUIs low battery LED script
# This becomes /usr/trimui/bin/low_battery_led.sh and is called by the system when the battery is low
# The only material difference at the time of writing it is that we enable/disable write
# access to the LEDs before operating on them.


# Enable write permissions on LED files
chmod a+w /sys/class/led_anim/*

case "$1" in
1 )
        echo "enter low battery"
        echo "FF0000 " >  /sys/class/led_anim/effect_rgb_hex_lr
        echo "30000" >  /sys/class/led_anim/effect_cycles_lr
        echo "2000" >  /sys/class/led_anim/effect_duration_lr
         echo "6" >  /sys/class/led_anim/effect_lr

        echo "FF0000 " >  /sys/class/led_anim/effect_rgb_hex_f1
        echo "30000" >  /sys/class/led_anim/effect_cycles_f1
        echo "2000" >  /sys/class/led_anim/effect_duration_f1
        echo "6" >  /sys/class/led_anim/effect_f1

        echo "FF0000 " >  /sys/class/led_anim/effect_rgb_hex_f2
        echo "30000" >  /sys/class/led_anim/effect_cycles_f2
        echo "2000" >  /sys/class/led_anim/effect_duration_f2
        echo "6" >  /sys/class/led_anim/effect_f2
        ;;
0 )
        echo "exit low battery"
        echo "0" >  /sys/class/led_anim/effect_lr
        echo "0" >  /sys/class/led_anim/effect_f1
        echo "0" >  /sys/class/led_anim/effect_f2
        ;;
*)
        ;;
esac