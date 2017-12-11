This is a work in progress.
It's licensed under GPL, errrhm, v2?

You can also fry the chip using it. If you break it - you own all the parts.

It is currently configured to use CY8C5667LTI-LP009, but you can change the chip in Project->Device Selector menu.
So it should work with CY8CKIT-059. Here's how.

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
* Columns. This is where things start to get complicated. Internally, there are 2 ADC blocks, connected to Cols[0-11] and Cols[12-23]. They're counted DOWN. Which means
  1. Columns must be balanced. So those 16 columns become 8 + 8. and
  2. FIRST n columns will not be read if there's less than 24 columns.
So, for 16-column keyboard, real columns will be Cols[4] - Cols[11] and Cols[16] - Cols[23]. To make things more interesting, column order is NOT reversed. Seriously, I need to consider making this part easier :)
* Rows. Simplest part. No restrictions.

So.

## Recommended pinout
* D0: P0[3]
* D1: P3[2]
* Rows: P0[0, 1, 5, 6, 7], P15[3, 4, 5]. Alternatively, P12 can be used to free more analog-capable pins, but watch for ExpHdr pins.
* Cols: P1[0-7], P2[0, 2-7], P3[0, 1, 3-7], P15[0, 1, 2].
* P0[2], P0[4] to +5V, P0[3], P3[2] to the ground.


Whew. Hopefully you're done with soldering now.

# Building
You'll need Windows machine. Mac with VirtualBox will do - but a couple of notes there.
You'll need to download [Virtualbox](https://www.virtualbox.org/wiki/Downloads) (v5.2.2 was used) - don't forget the extension pack - and a [windows image](https://developer.microsoft.com/en-us/microsoft-edge/tools/vms/) - I used "MSEdge on Win10". Download, unpack, click the .ovf, wait for import to complete. Then open settings (right-click, "Settings"), click "Ports", select "USB" tab, enable USB 2.0 controller, click the plug with blue dot. THIS WILL CONNECT ALL NEW USB DEVICES TO YOUR VM. Run the VM now. You have your windows development environment now. Download PSoC creator into VM, install (typical works just fine), and continue.
Linux with Virtualbox will probably work too - but not tested as I don't have any hardware running Linux and running virtualbox inside virtualbox is not something I plan to do anytime soon.

Download and install (PSoC Creator)[http://www.cypress.com/products/psoc-creator-integrated-design-environment-ide] - you actually can download without suffering akamai download manager. Typical install works just fine. v4.1 was used, though newer should work too.

## Bootloader
* Open PSoC Creator
* Open "CY8CKIT-059 Bootloader.cywrk" workspace
* Go to Project -> Device Selector menu, find and select "CY8C5888LTI-LP097".
* Press Shift-F6 ("Build"), wait for it to finish.
* Copy bootloader files **one level above** CommonSense directory. Yes. Above.
```
cp CommonSense\Bootloader.cydsn\Compiled\Bootloader.hex .
cp CommonSense\Bootloader.cydsn\Compiled\Bootloader.elf .
```
If there's no files at the location - in the left pane, right-click on "Workspace blah blah", select "Properties" and change "Active build configuration" to "Release".

Congratulations. Now for the main course!

## Firmware

Firmware is in model F mode - "normally low". Look into dma_core/globals.h to switch to beamspring mode - #define SWITCH_TYPE

* Open PSoC Creator, open CommonSense.cywrk workspace.
* Select Project -> Device Selector. Find and select "CY8C5888LTI-LP097".
* Open "Project "Firmware"" in the left pane, click "Pins" in "Design Wide Resources". You will see chip model and a table on the right. Assign pins according to plan.
* Press "Ctrl-F5" (Debug -> Program). The kit should be plugged in, of course. PSoC Programmer will demand to update KitProg - it explains how.

NOTE: Columns must be assigned so that last column is Cols[23]. If you have a 16-column matrix, physical leftmost column is Cols[8]. Extra columns can be left auto-assigned.

NOTE2: For dual-ADC left half must end with Cols[11], right half - with Cols[23].

--- it's time now to disconnect the board, peel that polyimide film off the micro USB socket and plug the host into it.

..I was able to plug both ends into the same USB hub (thinkpad docking station) without frying anything - but your mileage may vary. I DO NOT RECOMMEND IT, IT'S A STUPID DWARF TRICK.

You may optionally break the kitprog away in a symbolic gesture, but a) if I were you, I wouldn't and b) if you would - cut the laminate at the break line first. Not all the way (though you may if you have a dremel or a saw around), but enough so the board doesn't buckle so horribly. Oh, and soldering probably have to wait until this moment at least.

## FlightController

[Separate file](Qt-build/README.md)

# Configure keyboard

With empty EEPROM, keyboard won't work. You need to initialize it. There are config files in misc/ directory which should be a good starting point.
To load it into device, run FlightController, Config->Open, Config->Upload. BEWARE, thresholds may be set absolutely wrong!

## Configuring thresholds

Short version (for beamspring, invert direction, so max->min and negative adjustments): 
* Click "Key Monitor" button. 
* Click "Start!". Get the idea of levels that should be there - press keys, observe readings going up and down. Small numbers below 7-segment indicators are min/avg/max.
* Click "Stop!". Select "Max" into dropdown near the "reset" button, click "Reset", "Start!". 
* Wait 15 seconds or longer, while readings stabilize.
* click "Stop!". Click "Set thresholds". Close window.
* Click "Thresholds" in the main window. put 5 into adjuster spinbox, click "Adjust", click "Apply".
* Close threshold editor. Select "Config -> Upload" in menu. Test. Once you're satisfied with results, "Command -> Commit".

Thresholds should be set ~2x higher than most of the matrix settles on. For beamspring - probably 75% of the highest reading.
TEST SETTINGS BEFORE COMMITTING. It can be quite hard to get the keyboard back from the constantly spamming with keypresses! ("EMERGENCY STOP" button right after keyboard is connected helps)

## Configuring layouts
pretty straightforward. If thresholds are configured, pressed keys will be highlighted by "yellow highlighter" color.
Import and export will load and save to file. Structure is compatible with xwhatsit layout files.
