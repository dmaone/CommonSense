# DO NOT ASK ANY QUESTIONS ABOUT THIS.

# DO. NOT.

# SERIOUSLY.

# YOU HAVE BEEN WARNED.

If you plan to change anything (and I recommend at least bringing number of rows and columns in sync with your matrix) - skip this section.

**IF** you plan to use vanilla firmware (which will have **8x12** matrix, **default pinout** and will expect **buckling spring** switches) - you can get away with smaller download. You will still need a Windows machine though (see next section for "Windows machine" notes).

* Download [PSoC Programmer](https://www.cypress.com/documentation/software-and-drivers/psoc-programmer-archive) and install it.
* Plug the KitProg side in.
* Open "CommonSense/Capsense.cydsn/Compiled/Capsense.hex".
* IF the "Program" button is grayed out - go to "Utilities", click "Update Firmware". It usually takes ~30 seconds to update. After update, something like "KitProg/xxxxxxxxxxxxxxx" will appear in "Port Selection" window and the button will become blue.
* Click "Program". Takes about a minute to flash.

From now on, you _can_ update firmware using FlightController. Flightcontroller uses .cyacd files, which will be in the same directory as .hex
