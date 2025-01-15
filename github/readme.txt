This application is designed for the TrimUI smart Brick emulation handheld. 
There has been no testing of operational safety on any other hardware.
 

Stock OS: Place the "LedController.pak" folder in the "Apps" folder of your SDCARD and launch from the "Apps" menu.

MinUI: Place the "LedController.pak" folder in the "Tools/tg3040" folder of your SDCARD and launch from the "Tools" menu.


Controls: 

From the configuration menu...

DPAD UP/DOWN: Change the selected setting.
DPAD L/R: Change the value of the selected setting.
START: Access the start menu.
B/POWER: Exit the application.
L/R: Switch the active LED.


From the start menu...

DPAD UP/DOWN: Change the selected setting.
B/START: Return to config page.
A: Execute selected setting.


Q & A:

Q: What am I "Uninstalling"? 
A: When the application exits normally, I install a settings daemon (a service that 
runs on device startup) that sets the LEDs and prevents other processes from changing
them. If you decide you no longer want this functionality, the uninstall action removes
the daemon and returns LED functionality to stock.

Notes:
This is a free app made for fun over christmas break, hopefully you get some joy from messing with your LEDs. 
If you got this from anywhere other than https://github.com/pierceTee/TrimuiLEDController, 
you should follow that link and download the latest release. If you paid for this, get your money back ASAP. 