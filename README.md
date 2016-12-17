This is a work in progress.
It's licensed under GPL, errrhm, v2? GPL v2, because it uses tmk_core.

If you have PSoC Creator, Qt+Qt creator, CY8CKIT-059 prototype board and an IBM keyboard with 16x8 matrix (that includes practically any model F, but not beamsprings) - you may try it.
You can also fry the controller doing it. If you break it - you own all the parts.

If you are lucky^W^W do everything correctly - you will see the USB device with 2 interfaces and ADC readings in the utility (or wireshark) when you press the keys.
Also, with a bit of trial and error, you can make a working keyboard out of this thing.

READ THIS FILE TILL THE VERY END - it's not a linear story, it seems.

Soldering:
SOLDER IT SO THAT CHIP IS FACING AWAY FROM THE BACKPLATE.
It's in CAPS because I was stupid enough to solder it chip (and LED) _to_ backplate and have trouble seeing LED. I'm also lazy enough not to resolder.
Important warning done, here's to the business.

Ground to any ground point you find on the controller.
Rows to ports P2.0 P0.7 P2.2 P0.5 P2.4 P0.1 P2.6 P15.5 - looks strange, but it's really not. It's USB side of the board - and since the model F 122 keys pin step is 2x board step, I had to fit it that way. Also should reduce crosstalk a bit.
All row pins are reassignable - just avoid P0.2 - P0.4 and P3.2 - they have HUGE(relatively to the capacitance we're about to measure) 1uF bypass capacitors connected to them (see the kit documentation for details).
Columns to P1.[0-7] to P12.[0-7]. P1 is columns 0..7, P12 - 8..16.

"To the bat cave^W^W^W bootloader" pin is 2.3. Pull it low to go to bootloader. The LED will light steady blue while in bootloader.

Building the firmware:
1) build and program bootloader (CY8CKIT-059 Bootloader.cywrk). Basically connect the kit, click the workspace, wait for it to load and press Ctrl-F5. It will demand to update KitProg - it explains how.
IMPORTANT - don't forget to set "Active build configuration" to "Release" - "Debug" has lots of settings missing.

--- it's time now to disconnect the board, peel that polyimide film off the micro USB socket and plug the host into it.
..I was able to plug both ends into the same USB hub (thinkpad docking station) without frying anything - but your mileage may vary. I DO NOT RECOMMEND IT, IT'S A STUPID DWARF TRICK.
You may optionally break the kitprog away in a symbolic gesture, but a) if I were you, I wouldn't and b) if you would - cut the laminate at the break line first. Not all the way (though you may if you have a dremel or a saw around), but enough so the board doesn't buckle so horribly. Oh, and soldering probably have to wait until this moment at least.

2) copy C:\Projects\CommonSense\Bootloader.cydsn\CortexM3\ARM_GCC_493\Release\Bootloader.hex and Bootloader.elf to repo root directory.

3) build the firmware (Shift-F6). It depends on bootloader files in location from item 2.

4) go Tools->Bootloader host. press "Filters" button, VID 04B4, PID B71D. Select the bootloader, select the file you just created (Firmware.cydsn\CortexM3\ARM_GCC_493\Release\Firmware.cyacd), flash.
4.1) Don't forget to momentarily pull down the pin 2.3 so that the board is in bootloader. It will not be visible otherwise.

5) After programming completes - there will be 5 short blinks and then LED will go dark. If you press the key - you'll see the LED flashing.

Building the application:

PREREQUISITE: hidapi library must be built and path to it specified in .pro file.

1) go to FlightController catalog
2) click "FlightController.pro"
3) press Ctrl+R
4) Wait for the window to appear.
5) Click "Key monitor"
6) Click "Start" in the window that appears.

Now press the buttons on the keyboard. You should see how readings slowly rise. Slowly - because glitch filter is "rolling average"

The utility can, actually, program things to make a working keyboard from the device. I hope it's intuitive! :)

That's all, folks.
