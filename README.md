This is a work in progress.
It's licensed under GPL, errrhm, v2?

You can fry the chip using it. If you break it - you own all the parts.

!!!CAUTION!!! Make sure your build environment works - build the firmware first, without any modifications!

# Soldering

SOLDER IT SO THAT CHIP IS FACING AWAY FROM THE BACKPLATE.
It's in CAPS because I was stupid enough to solder it chip (and LED) _to_ backplate and have trouble seeing LED. I'm also lazy enough not to resolder.
Important warning done, here's to the business.

Ground to any ground point you find on the controller. Doesn't matter much.
What matters is that you connect BOTH plates and PCB ground to the controller ground.

You'll need 35 pins. That's not much more than the kit has - so choose the layout wisely.

## RECOMMENDATIONS
* P0[2] and P0[4] MUST be electrically connected to +5V (Well.. that's one STRONG recommendation. Not following it will lead to WILD readings, because those pins are ADC voltage references.).
* P12(all pins of it) cannot be used for analog connections. That means you can only assign Rows to those pins.
* D0 and D1 MUST be electrically connected to the ground. Use P0[3] for D0 and P3[2] for D1 (you can't use those pins for anything else anyway - they have 1uF capacitors connected to them). Can be any pins except P12[x] though.
* It is better to separate Rows and Cols by a pin electrically connected to the ground, but not necessary. If you use adjacent pins - one matrix cell will have higher readings, that's all.
* Rows. Simplest part. No restrictions.
* Columns. Columns must be assigned so that last column is Cols[23]. If you have a 16-column matrix, physical leftmost column is Cols[8]. Extra columns can be left auto-assigned.

If you have dual-ADC version - things start to get complicated. ADC blocks are connected to Cols[0-11] and Cols[12-23]. They're counted DOWN. Which means
  1. Columns must be balanced. So those 16 columns become 8 + 8. and
  2. FIRST n columns will not be read if there's less than 24 columns.
So, for 16-column keyboard, real columns will be Cols[4] - Cols[11] and Cols[16] - Cols[23]. To make things more interesting, column order is NOT reversed. Seriously, I need to consider making this part easier :)

So.

## Recommended pinout
* D0: P0[3]
* D1 (if exists): P3[2]
* Rows: P0[0, 1, 5, 6, 7], P15[3, 4, 5]. Alternatively, P12 can be used to free more analog-capable pins, but watch for ExpHdr pins.
* Cols: P1[0-7], P2[0, 2-7], P3[0, 1, 3-7], P15[0, 1, 2].
* P0[2], P0[4] to +5V, P0[3], P3[2] to the ground.
* ExpHdr (solenoid/LED connector) is configured to blink the kit's LED. Leave it assigned as is, unless you really know what you're doing.

Whew. Hopefully you're done with soldering now.

# Building
You'll need Windows machine. Mac with VirtualBox will do - but a couple of notes there.
You'll need to download [Virtualbox](https://www.virtualbox.org/wiki/Downloads) (v5.2.2 was used) - don't forget the extension pack - and a [windows image](https://developer.microsoft.com/en-us/microsoft-edge/tools/vms/) - I used "MSEdge on Win10". Download, unpack, click the .ovf, wait for import to complete. Then open settings (right-click, "Settings"), click "Ports", select "USB" tab, enable USB 2.0 controller, click the plug with blue dot. THIS WILL CONNECT ALL NEW USB DEVICES TO YOUR VM. Run the VM now. You have your windows development environment now. Download PSoC creator into VM, install (typical works just fine), and continue.
Linux with Virtualbox will probably work too - but not tested as I don't have any hardware running Linux and running virtualbox inside virtualbox is not something I plan to do anytime soon.

Download and install (PSoC Creator)[http://www.cypress.com/products/psoc-creator-integrated-design-environment-ide] - you actually can download without suffering akamai download manager. Typical install works just fine. v4.1 was used, though newer should work too.

## Firmware
* Open PSoC Creator
* File -> Open -> Project/Workspace "CommonSense.cywrk".
* IMPORTANT: Select "Release" in the  rightmost combo box on the top toolbar that says "Debug".
* Go to Project -> Device Selector menu, find and select "CY8C5888LTI-LP097". It's likely selected already.
* Press F6 to build everything.

Now it's a good time to make your firmware customizations. CommonSense/Firmware.cydsn/config.h is a config file. You can set number of rows, columns and layers there, as well as switch type.
But, again: _build without any changes first_!

* Open "Project "Firmware"" in the left pane, click "Pins" in "Design Wide Resources". You will see chip model and a table on the right. Assign pins according to plan.
* Press "Ctrl-F5" (Debug -> Program). The kit should be plugged in, of course. PSoC Programmer will demand to update KitProg - it explains how.

--- it's time now to disconnect the board, peel that polyimide film off the micro USB socket and plug the host into it.

..I was able to plug both ends into the same USB hub (thinkpad docking station) without frying anything - but your mileage may vary. I DO NOT RECOMMEND IT, IT'S A STUPID DWARF TRICK.

You may optionally break the kitprog away in a symbolic gesture, but a) if I were you, I wouldn't and b) cut the laminate at the break line first. Not all the way (though you may if you have a dremel or a saw around), but enough so the board doesn't buckle so horribly. Oh, and soldering probably have to wait until this moment at least.

## FlightController

[Separate file](Qt-build/README.md)

# Configure keyboard

With empty EEPROM, keyboard won't work. You need to initialize it. There are config files in misc/ directory which should be a good starting point.
To load it into device, run FlightController, Config->Open, Config->Upload. BEWARE, thresholds may be set absolutely wrong!

## Hardware configuration hints
* If key monitor only shows zeroes, no matter which keys you press - set ADC resolution higher.
* If even at 12 bits it still doesn't work - increase charge delay. Recommended setting is 18.
* If you see double actuations on keypress - set debouncing higher.
* If several keys fire at once - set longer discharge delay. I found that 180 (10us) works pretty well.
* If you haven't touched ExpHdr pins - settings mode to "Solenoid" and drive time to 100 will make a LED on the kit blink with every keypress (not in setup mode).

## Configuring thresholds
!!!NOTE!!! If you see double presses on some keys, set threshold higher - just under the steady pressed state. Some keys have physical bounce and there are two peaks. Controller is too fast and sees this as 2 keypresses.

Short version (for beamspring, invert direction, so max->min and negative adjustments): 
* Click "Key Monitor" button. 
* Click "Start!". Get the idea of levels that should be there - press keys, observe readings going up and down. Small numbers below 7-segment indicators are min/avg/max.
* Click "Stop!". Select "Max" into dropdown near the "reset" button, click "Reset", "Start!". 
* Wait 15 seconds or longer, while readings stabilize.
* click "Stop!". Click "Set thresholds". Close window.
* Click "Thresholds" in the main window. put a small positive value (see below) into adjuster spinbox, click "Adjust".
* Click "Apply".
* Close threshold editor. Select "Config -> Upload" in menu. Test. Once you're satisfied with results, "Command -> Commit".

Thresholds should be set ~2x higher than most of the matrix settles on. For beamspring - probably 75% of the highest reading.
TEST SETTINGS BEFORE COMMITTING. If you get thresholds wrong - there will be a red light in the status bar and keyboard refuse to produce output.

## Configuring layouts
Pretty straightforward. If thresholds are configured, pressed keys will be highlighted by "yellow highlighter" color.
Import and export will load and save to file. Structure is compatible with xwhatsit layout files.
