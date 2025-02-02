# TrimUI LED Controller

### An application to configure the LEDs on the TrimUI Brick (smart pro support WIP). 
#### *Configure all LED brightness, color, and effect!*

![Configure all LED brightness, color, and effect!](./github/led_controller_main.gif)

## Installation

This application is designed for the TrimUI smart Brick emulation handheld. There has been no testing of operational safety on any other hardware.

- **Stock OS:** Place the `LedController.pak` folder in the `Apps` folder of your SDCARD and launch from the `Apps` menu.
- **MinUI:** Place the `LedController.pak` folder in the `Tools/tg3040` folder of your SDCARD and launch from the `Tools` menu.

## Usage Guide

### Controls

**From the configuration menu:**

- **DPAD UP/DOWN:** Change the selected setting.
- **DPAD L/R:** Change the value of the selected setting.
- **START:** Access the start menu.
- **B/POWER:** Exit the application.
- **L/R:** Switch the active LED.

**From the start menu:**

- **DPAD UP/DOWN:** Change the selected setting.
- **B/START:** Return to config page.
- **A:** Execute selected setting.

### Menu Options

- **Sync LED colors:** Match all LED colors to the currently selected LED.
- **Turn ON all LEDs:** Enable all LEDs by setting their brightness to the maximum value.
- **Turn OFF all LEDs:** Disable all LEDs by setting their brightness to 0.
- **Enable/Disable extended colors:** Toggle the ability to select colors from an extended range of RGB values.
- **Enable/Disable low battery warning:** Toggle the default OS behavior of flashing LEDs when the battery falls below 15%.
- **Uninstall:** Uninstall all services and system files, turning off the LEDs on exit.


## Development Guide

All development work has been performed in a Docker container from [arm64-tg3040-toolchain](https://github.com/pierceTee/arm64-tg3040-toolchain.git) running on a Windows host machine. Theoretically, this should be cross-platform, but practical applicability is left to the reader.

If you want everything handled for you, run `dev_scripts/make-all-packages` from the root of the project directory, then navigate to the corresponding release folder.

### To Build Manually:

1. Pull submodules and build the `toolchains/arm64-tg3040-toolchain` Docker container.
2. Run `dev_scripts/run-container.sh` to enter into the `arm64-tg3040-toolchain` image.
3. Run `make` to build the project, then `make package` to package it into the `release/aarch64` folder.
4. To test the application:
  - **On your host machine:** Select a project from `release/<host_architecture>/` (e.g., `release/<host_architecture>/LedController`) and run the application from the command line.
  - **On your device:**
    - **MinUI:**
     1. Copy the project release `release/aarch64/LedControl.pak` to `<SDCARD_ROOT>/Tools/tg3040/`.
     2. On the device, navigate to `Tools`, find your `LedController` entry, and launch.
    - **Stock OS:**
     1. Copy the project release `release/aarch64/LedController` to your `<SDCARD>/Apps/`.
     2. On the device, navigate to your `Led Controller` entry in the "apps" section and launch.

Although alternatives are easily attainable, the project is structured with the idea that the user will compile in the Docker container found in toolchains. From this container, you can run the application as if it were on the TrimUI device given you've connected your host display to the container (see `dev_scripts/run-container.sh`). You can use a tool like `gdb` to debug the application from your host machine by navigating to your host machine's architecture release directory and manually launching with your desired debugger. Just be aware that while all versions of the app *should* behave the same, this isn't guaranteed to be the same behavior you see on your TrimUI device.

## Troubleshooting

- **Dependency issues:** Try running `make deps`.
- **Compiler issues:** Try running `dev_scripts/setup-toolchain.sh` followed by `dev_scripts/setup-env.sh`.
- **Docker issues:** Try running `./toolchains/arm64-tg3040-toolchain/make shell` then `./dev_scripts/run-container.sh`.

## References/Findings

All logic is derived from the help file found in `/sys/class/led_anim/`

```
[TRIMUI LED Animation driver]
   max_scale : maximum LED brightness in dec [0 ~ tg5040 limit brightness 60]
   frame     : raw frames for total 23 XRGB 32bpp data
   frame_hex : frames for total 23 XRGB 32bpp data in hex format "RRGGBB RRGGBB RRGGBB ... RRGGBB " end with space.

[usage of anim to function]
   effect_lr: Left and right joystick LEDS effect type.
   effect_l: Left joystick LEDS effect type.
   effect_r: Right joystick LEDS effect type.
   effect_m : middle LED effect type.
  (effect_x for a trigger of effect start)
   effect_names : show the effect types description.
   effect_duration_lr: Left and right joystick LEDS animation durations.
   effect_duration_l: Left joystick LEDS animation durations.
   effect_duration_r: Right joystick LEDS animation durations.
   effect_duration_m : middle LED effect duration.
   effect_rgb_hex_lr: Left and right LED all target color in format "RRGGBB " end with space.
   effect_rgb_hex_l: Left LED all target color in format "RRGGBB " end with space.
   effect_rgb_hex_r: Right LED all target color in format "RRGGBB " end with space.
   effect_rgb_hex_m: Middle LED target color in format "RRGGBB " end with space.
   effect_cycles_lr: Left and right joystick LEDS animation loops.
   effect_cycles_l: Left joystick LEDS animation loops.
   effect_cycles_r: Right joystick LEDS animation loops.
   effect_cycles_m : middle LED effect loops.
  (cycles value: 0 for stop, -1 for endless loop, > 0 for loop times)
   effect_enable   : toggle of anim to function

[usage of framebuffer animation function !!Did not finish yet!!]
   anim_frames:       raw frames for total XRGB 32bpp data, buffer length 10 sec@60fps, 23 data per frame.
   anim_frames_hex:   same as anim_frames and use hex format "RRGGBB RRGGBB RRGGBB ... RRGGBB " end with space.
   anim_frames_cycle: animation loops count
   anim_frames_enable : toggle of frames anim function
   anim_frames_override_m_enable: toggle of middle LED in frames anim function.
```

The tricky part was getting a daemon running on boot. First, `systemctl` isn't installed on the device so it's not as easy as starting/stopping with that. Daemons in `/etc/init.d` also use a different format than I've seen before, they're very primitive and seem to order themselves by a `START` variable (with 0 being first and running in order from there). I've done my best to run the daemon at the right time to make the changes feel seamless (i.e., the OS doesn't control the LEDs at any point) by placing the order just before the `runtrimui` service. Run `grep -H "START=" /etc/init.d/* | awk -F 'START=' '{print $2, $0}' | sort -n | cut -d' ' -f2-` on the device to see the order of boot services for more context.

Getting a functioning daemon was a bit of a pain but here are a few things that helped me see where I was going wrong: 
- The brick uses an OpenWrt `/etc/rc.common` script to 'link' 'start', 'stop', 'restart', 'reload', 'boot', 'shutdown', and 'enable' services. So using this script will ensure compatibility with whatever the OS is expecting. 
- Check the examples in `/etc/init.d` a few times and cross-reference with what `/etc/rc.common` things are a little fast and loose (`printf` and `echo` both work? so do `start`, `start_service`, `start_service_remote`, and `start_service_daemon` [see `/etc/init.d/log`]). I should probably look into this standard to better understand what's going on under the hood.

1/12/2025: Turns out there's a directory `usr/trimui/bin` (should have looked at the minUI launch scripts a bit more thoroughly). In there is a script `usr/trimui/bin/init_leds.sh`. I'd bet we can just override this for an easy install method. There's a `usr/trimui/bin/low_battery_led.sh` that I should also overwrite to lock/unlock permission in toggling the LEDs.

P.s: there's loads more scripts in here, this might be the key to unlocking more functionality. 
UPDATE: overwriting this script didn't seem to do anything :(

Looks like MinUI actually does exactly what I'm thinking of doing here, they move `usr/trimui/bin/runtrimui.sh` `usr/trimui/bin/runtrimui-original.sh` and replace it with `skeleton/BOOT/trimui/app/runtrimui.sh` which then launches MinUI's `.tmp_update/updater` (or `runtrimui-original.sh` if the SDCARD isn't found). 

`usr/trimui/bin/trimui_inputd` is the input daemon written in C so maybe this is where runtime daemons go?

Tons of more interesting things in `usr/trimui`, all the stock apps are in `usr/trimui/apps` and can easily be moved to the SDCARD for use in other OSs, alternatively we can move our app in here for a "permanent" install. `usr/trimui/gamecontrollerdb.txt` seemingly has the mapping of controllers to SDL inputs, if we wanted to add support for a certain controller, it would go there.

`usr/trimui/lib` has all the installed libraries listed here:

```
libSDL-1.2.so.0             libSDL_image-1.2.so.0
libSDL-1.2.so.0.11.4        libSDL_image-1.2.so.0.8.4
libSDL2-2.0.so.0            libSDL_mixer-1.2.so.0
libSDL2-2.0.so.0.3000.8     libSDL_mixer-1.2.so.0.12.0
libSDL2_image-2.0.so.0      libSDL_ttf-2.0.so.0
libSDL2_image-2.0.so.0.2.3  libSDL_ttf-2.0.so.0.10.1
libSDL2_mixer-2.0.so.0      libgamename.so
libSDL2_mixer-2.0.so.0.0.1  libshmvar.so
libSDL2_ttf-2.0.so.0        libtmenu.so
libSDL2_ttf-2.0.so.0.14.1
```

I wonder if we could swap in some binaries to support more applications?

## Final Thoughts

Yes, this project is totally overkill. This can be done MUCH simpler as controlling the LEDs is as simple as `echo`-ing an integer into a few text files. But that's no fun... I was on winter break and had been meaning to dip my toes back into firmware/OS development, this project helped scratch that itch.

Much of the project time was spent building a strong framework for myself and others to continue dev work on ARM-based Linux handhelds in the future. Those tools include but aren't limited to: a plug-and-play Docker container with scripts to easily build/deploy/test applications from your host machine and a base SDL library that handles all the display setup and input processing.

This project was a ton of fun, it helped me better understand the wild world of custom operating systems on retro handhelds, sprites and sprite sheet rendering with SDL2, device-specific processes of the TrimUI Brick, the current state of generative-AI SWE tools, and much more.

I hope being able to control your LEDs brings you even a fraction of the joy making this project has brought to me.

Best,  
Pierce

## Special Thanks 

- [Shaun Inman](https://github.com/shauninman): Their dev toolchains, and MinUI as a whole were instrumental in building this project.
- [ro8inmorgan](https://github.com/ro8inmorgan): Their [LedControl](https://github.com/ro8inmorgan/LedControl) application helped me fix some bugs/functionality in this application.
- Retro Handhelds discord users: `Heef`, `Richard(RJNY)`, and `seki`. The app wouldn't be half as polished without their testing and feedback.
- [skiselkov](https://github.com/skiselkov): For their contribution to the application.

## TODO 

- Smart pro support.
- Separate core SDL functionality to its own repo to import as a submodule for future apps?

