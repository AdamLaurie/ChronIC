ChronIC - Chronos Integrated Commander
======================================

A wearable Sub-GHz RF hacking tool.

Latest version:

  https://github.com/ApertureLabsLtd/ChronIC

Based on the EZ430 Chronos project from Texas Instruments:

  http://processors.wiki.ti.com/index.php/EZ430-Chronos

This tool allows transmission of arbitrary RF data from the wristwatch
component of the Chronos dev kit:

  http://www.ti.com/tool/ez430-chronos&DCMP=Chronos&HQS=Other+OT+chronos

A worked example of hacking my doorbell can be found here:

  http://adamsblog.aperturelabs.com/2013/03/you-can-ring-my-bell-adventures-in-sub.html

INSTALLATION
============

Firmware
========

The code is designed to compile with the free size-restricted version
of CCStudio from TI:

  http://www.ti.com/tool/ccstudio

To achieve this, much of the original functionality is commented out, but
if you have access to the unrestricted compiler it should be easy enough
to re-enable.

I do my main development under Linux but it's also tested and working under
Windows.

Checkout latest code from github:

  git clone git://github.com/ApertureLabsLtd/ChronIC.git

Start CCStudio and import the project from the CCS sub-folder of the repo:

  File/Import.../Existing CCS Eclipse Projects

Build:

  Project/Build Project

Note that if you don't want to go to the trouble of building drom source, I've 
included the current 433 and 868 compiled versions in the '433MHz - Unrestricted CCS Platinum'
and '868MHz - Unrestricted CCS Platinum' directories.

Install via RF Bootloader with Chronos Control Center:

  Windows: http://www.ti.com/lit/zip/slac341

  Linux: http://www.ti.com/lit/zip/slac388

Python CLI
==========

Change directory to the 'python' folder in the git repo.

  sudo python ./setup.py install

Create a file in /etc/udev/rules.d called 20-chronos.rules with the following
line in it:

  UBSYSTEMS=="usb" ATTRS{idVendor}=="0451" ATTRS{idProduct}=="16a6" MODE:="0666" SYMLINK+="CHRONOS"

Reload the udev rules:

  sudo udevadm control --reload-rules

This will automatically create the device /dev/CHRONOS when you plug in the 
Chronos USB dongle, and make it non-root accessible. This is the default
device chronic-cli.py will try to open, but something else can be specified
on the command line if you prefer not to play with udev.

USE
===

In normal use the watch has two main 'menus': one for the top row, and one
for the bottom. Pressing the * button cycles through the top row menus and
pressing # cycles the bottom row. When a particular menu item is shown, 
pressing the UP or DOWN buttons activates its function. In ChronIC, this
is normally 'selecting' that menu item, but when the normal 'time/date'
items are shown, pressing UP or DOWN will always transmit the data associated
with each button. Selecting a menu item will display an 'R' symbol against that
item when cycling through, showing that it is currently active.

To associate specific data with a button, you can either transmit the data 
to the watch using SYNC mode (selected on the lower menu) and the python
helper app 'chronic-cli.py', or you can choose a pre-programmed data set
which includes correct timing, frequency, modulation etc. from the upper
menu. An example called 'BELL' will select the appropriate data and
transmission settings to ring my doorbell, should you happen to be in my
neighbourhood. :)

SETTINGS
========

The following parameters can be set with the python cli:

  BAUD <Rate>                   RF Modem baudrate in Hz.

  BYRON                         Configure for Byron doorbell emulation (433).

  DELAY <1-255>                 Milliseconds delay between each REPEAT.

  DOWN <HEX> <HEX> <HEX>        DATA to be sent by DOWN button. Three sequences of up to 63
                                bytes each.

  FREQ <Hz>                     Frequency e.g. 433920000.

  FRIEDLAND                     Configure for Friedland doorbell emulation (868).

  MAN <ON|OFF>                  Manchester Encoding ON or OFF.

  MOD <FSK|GFSK|OOK|MSK>        Modulation:

                                FSK -  Frequency Shift Keying
                                GFSK - Gaussian Frequency Shift Keying
                                OOK -  On-Off Keying (in Amplitude Shift Keying mode)
                                MSK - Multiple Frequency Shift Keying

  PORT <Port>                   The serial port your Chronos USB dongle lives on (default /dev/CHRONOS).
                                This should be set as the first command if the default does not exist.

  PULSE <Width>                 RF Modem baudrate expressed as pulsewidth (e.g. a pulsewidth
                                of 0.000320 seconds is 3124.23 Hz). In ASK/OOK mode, the 
                                baudrate is generally the pulsewidth of the shortest pulse
                                found in the original signal.

  REPEAT <1-255>                Number of times to send DATA SEQUENCES when button pressed.

  RUKU                          Configure for Ruku garage door opener emulation (433).

  SERIAL <Baudrate>             USB dongle comms baudrate (default 115200).

  TIME                          Set TIME/DATE to match PC.

  UP <HEX> <HEX> <HEX> 		DATA to be sent by UP button. Three sequences of up to 63 
                                bytes each.

  EXIT                          Exit SYNC mode on watch.

Examples:

  Send a fairly well known signal:

    chronic-cli.py FREQ 433920000 MAN OFF MOD OOK PULSE .008 DELAY 255 REPEAT 5 UP aaaa0000aaaa0000aaaa0000aaaaaaaaaaaa0000aaaaaaaaaaaa0000aaaaaaaaaaaa0000aaaa0000aaaa0000aaaa0000 '' ''  EXIT

  Manually configure to ring my doorbell:

    chronic-cli.py FREQ 433920000 MAN OFF MOD OOK PULSE .000320 DELAY 0 REPEAT 60 UP 2C92496DB2000000 '' '' DOWN 2C92496DB2000000 '' '' EXIT

The helper app also includes a couple of examples showing how easy it is to hardwire specific configurations. These
are 'BYRON' and 'RUKU', which are my doorbell and a simple garage door opener respectively.

Enjoy!
Adam

