# Cortron p/n 35-500079

## Device description
"Medium ITW magnetic valve", I guess. Important part is it initially refused to work - half of the keys flat out refused to fire, 20% were responsive to rocking them (press and nudge right - they fire, nudge left - they don't).

Turns out the leads on those switches are made of some strange material which refuses to be wetted by standard 37Pb63Sn solder.

Detection: touch the solder joing with a soldering iron (give it some flux up front, because that thing was made 50 years ago, it has oxide CRUST on it, not "film") - and observe solder JUMPING from the lead.

It took copious amounts of RSF-R80-2 RMA flux and a scraper tool to convince it to take some tin. The tool (and what happens after heating the joint) looks like this: https://www.instagram.com/p/B251_1UjHZ7/. It is made from a diode because that's what I had on hand.

Leads had to be bathed in flux and scraped for 10-15 seconds, repeatedly - with new flux applied to the lead every round.

I am NOT joking about scraping.

Even with scraping, after washing away the flux residue I see lots of "cold joints" and even holes in the solder between lead and the pad - but it works, and it's not my keyboard so I'm not going to switch to OA flux (or even IA/tip tinner) and have a chance to ruin the PCB. RMA flux just doesn't cut it, even after 30 seconds of boiling in it.

Unfortunately, I didn't measure resistance before the resoldering operation, so I don't know how large it was.

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

Comparator (not amplifier as in patent! LM319N in this case) generates an extremely short (and very unstable because of power sag:
 ([SHOCK VIDEO](https://www.instagram.com/p/B0Ef6aLliQX/)) pulse - ~150ns. Latch (another half of 74LS00N) takes care of it,
presenting MCU with the signal depicted as IN: on previous diagram - from the "ON" of positive strobe to the "OFF" of negative one.

![Strobe, sense line, comparator output](OriginalTimings/Strobe_and_sense_200ns.png)

## Direct drive experiments
All driving is performed with strong drive, via 1kΩ current-limiting resistor. Note that datasheet specifies max. source current
for PSoC5 LP GPIO at 4mA, so if you want to repeat this and reduce this resistor - consult your datasheet and don't fry your GPIOs.

### No load
![No load, released](DirectDrive/No_Load_Released.png)
![No load, pressed](DirectDrive/No_Load_Pressed.png)

### 1000pF load
![No load, released](DirectDrive/No_Load_Released.png)
![No load, pressed](DirectDrive/No_Load_Pressed.png)

## First prototype
79.5MHz main clock (12.5ns/tick), strong drive, "slow" slew rate, no current-limiting resistors.
Not sure if I can get away with that long-term (worried about GPIOs being fried in a year of such abuse) - but 330R drops the pulse height to ~5mV. Comparator - somewhat surprisingly - triggers on that with VDACs set to "1"
(supposedly 4mV) when trimmed (Trim seems to be ~1mV/click). But with no resistors it's ~50mV, so 10 or even 15 still triggers.

So, with 4 CLK pulse length things look like this:
![released](DirectDrive/4_clk_pulse_pressed.png)
![released](DirectDrive/4_clk_pulse_released.png)
The dip in the driving pulse is caused by slew rate limiter. If it's not there - voltage spikes to almost 5V, and readouts become VERY noisy.

Duty cycle is extremely small - hopefully that will be enough to not shorten GPIOs life.
![released](DirectDrive/4_clk_pulse_pressed_overview.png)

Just for the reference - 20 CLK pulses. Things start to discharge in about 60ns, and signal is gone in 200ns, so ADCing that is not feasible.
![pressed](DirectDrive/20_clk_pulse_pressed.png)
![released](DirectDrive/20_clk_pulse_released.png)

A bit longer window:
![pressed](DirectDrive/20_clk_pulse_pressed_zoomed_out.png)
![released](DirectDrive/20_clk_pulse_released_zoomed_out.png)

## Release version
Covered in the main inductive README.
