# Cortron p/n 35-500079

### Driving logic
Oscillograms are [here](OriginalTimings/)

SN74145 is used for a Reason.
The Reason is all 10 outputs are essentially shorted together - well, technically, they're connected via a wire wound via all those ferrite cores, so there's some impedance, but inductance is low and the resistance is abysmally low, so a lot of active power is dissipated.
So much, in fact, that voltage sags to 3.8-3.9V at the USB plug, and 74145 itself is slightly warm.
RMS at the plug is 4.42-4.44V, which is very close to USB spec's absolute minimum of 4.4V.

Other than that - in my devices 7442 (rows) is driven normally, 0 to 9 (bits 4,5,6,7 - bit 0 being highest frequency), but 74145 is driven in an interesting way: bits 0 and 1 are coming from the MCU, but bits 2 and 3 are modulated by a NAND gate (half of 74LS00N) with positive strobe (φ2 in US3978474A AKA P1 in US4263582).
MCU drives all 16 values, but only 4,5 and 8 to 15 bring any of the outputs low. So, waveform of 1 row looks like this:
```
70*us/~~~, subdivisions not to scale.
CLK:   0  1  2  3  4  5  6  7   8  9 10 11 12 13 14 15

A:   ___~~~___~~~___~~~___~~~ ___~~~___~~~___~~~___~~~

B:   ______~~~~~~______~~~~~~ ______~~~~~~______~~~~~~

C:   ~~~~~~~~~~~~~_~~_~~_~~_~ ~~~~~~~~~~~~~_~~_~~_~~_~

D:   ~~~~~~~~~~~~~~~~~~~~~~~~ ~_~~_~~_~~_~~_~~_~~_~~_~

Out: ~~~~~~~~~~~~~_~~_~~~~~~~ ~_~~_~~_~~_~~_~~_~~_~~_~

```
*) first 63 full periods of A (___~~~) are 70us, followed by 17 periods of 98us, giving full cycle time of 3.3+8.8=12.1 milliseconds.

The strobes themselves are a bit different from the patent - there's a positive strobe (2 lines, absolutely identical) and negative strobe. They are located in the middle of a pulse (20us + strobe + 20us or 40us + strobe + 20us) and shifted 5us front, 10us back:
```
5us/~
PS: __~~~~~_

NS: ~____~~~

IN: ~~___~~~
```

Comparator (not amplifier as in patent! LM319N in this case) generates an extremely short (and very unstable because of power sag: ([SHOCK VIDEO](https://www.instagram.com/p/B0Ef6aLliQX/)) pulse - ~150ns. PFN (another half of 74LS00N) takes care of it, presenting MCU with the signal depicted as IN: on previous diagram - from the "ON" of positive strobe to the "OFF" of negative one.
![Strobe, sense line, comparator output](OriginalTimings/Strobe_and_sense_200ns.png)

## Direct drive experiments
All driving is performed with strong drive, via 1kΩ current-limited resistor. Note that datasheet specifies max. source current for PSoC5 LP GPIO at 4mA, so if you want to repeat this and reduce this resistor - consult your datasheet and don't overload your GPIOs.

### No load
![No load, released](DirectDrive/No_Load_Released.png)
![No load, pressed](DirectDrive/No_Load_Pressed.png)

### 1000pF load
![No load, released](DirectDrive/No_Load_Released.png)
![No load, pressed](DirectDrive/No_Load_Pressed.png)
