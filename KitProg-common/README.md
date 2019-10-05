# KitProg-based converters (currently sun and ADB)

## Flashing 
KitProg is put into bootloader mode by pressing the button while inserting into USB. 

WARNING - pressing the button when it's powered will switch it into CMSIS/DAP mode which you don't want.

Some KitProgs don't respond well to "enter bootloader" command from FlightController. Symptom - rapidly blinking green LED.

If this is the case - press the button on KitProg BEFORE clicking "Update FW" button. If KitProg already runs CommonSense - it will switch to bootloader. If not - see above warning.
