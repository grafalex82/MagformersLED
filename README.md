# MagformersLED

WS2812 based LED module for Magformers. It provides a few flashing patterns including rainbow, color fills, and stars. It also has a few vechicles beacon modes (e.g. police) to be installed on car models. 
It can also serve as a toy traffic lights.


# Schematics

Schematics and PCB design can be found [here](https://easyeda.com/editor#id=d2c35121a0d542c1a6841c18a09a0e9e|6a5e1ef72c0a42daac36ed27372bff6e)

Module is based on ATTiny85, USB related schematics is taken from digispark85.

Battery charger is based on PT1502. Supplemental chematics is taken from datasheet.

# Case

Case can be printed with regular FDM printer. Models can be downloaded from thingiverse.

Models sources are at https://cad.onshape.com/documents/db0b892b322747d7bc535bdc/v/fcce0f163beeab5202f6b8d5/e/5b816cb997ed4dcfc992b5dd

# Building the code

To flash the code device requires a bootloader. Device is using https://github.com/micronucleus/micronucleus.git bootloader with [custom configuration](bootloader/bootloaderconfig.h). 
Refer to micronucleus documentation how to build and upload bootloader. Most probably you will need to play with BOOTLOADER_ADDRESS parameter - it is well explained in [Makefile.inc](bootloader/Makefile.inc)

Main firmware code can be built in a regular way in Arduino IDE. However it requires installing didispark ATTin85 boards and USB drivers first. Please google for 'integrating digispark to ArduinoIDE'.

# Controls

The device has just one button.
 - 2 seconds press powers up the device
 - short press switches modes
 - long press (over 5 seconds) power downs the toy


