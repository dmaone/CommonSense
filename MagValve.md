[![SO YOU HAVE COME.](https://img.youtube.com/vi/k0VvMUD5vBY/0.jpg)](https://www.youtube.com/watch?v=k0VvMUD5vBY)

Remember that guy? Yeah. This is _that_ level of difficulty.

There's no matrix monitor - the programmable hardware is totally different and is, essentially, a 4-channel xwhatsit reimplementation, the only deviation being a separate DAC (hence threshold) per channel, each channel sitting behind a 4-channel multiplexor.

![scanner](docs/MagValve/scanner.png)

So it's back to blind tuning, and there's no autotune support xwhatsit util had. Partly because I'm a bit short on time recently, but largely because out of 3 test devices all 3 worked with same thesholds. I was able to literally solder a different keyboard to the same controller and it just worked with the thresholds from another - and to program a new controller, solder it to the third keyboard and just needing to correct for just a couple of keys, problem being "spacebar triggers if you press basically anything else".

Not much is known about what's _really_ going on inside the matrix, but I intend to get a bigger scope and figure this out in a couple of weeks while I still have access to the hardware.

# Prepping the board
* Take high-res digital photos of both sides of your keyboard. High-res enough for traces to be 5px wide. You will need those when things go wrong and you'll need to trace things. <br>Bonus points if you can fit all the board on one photo - stitching multiple photos is an ungrateful job.

Reference photos (too large to be inlined): [Cortron 35-500079](docs/Cortron_35-500079_map.jpg), [Harris](docs/Cortron_125-500451_map.jpg).
Black is ground, colors are rows/columns. They are messy, because it's not easy to find 20 different colors and I had to resort to patterns.

* Remove the drivers: 74145(s) and 7442. Hot air is your weapon of choice, but solder wick or even dremel work just as well - you won't need those anymore. Just try not to damage the pads.
* Find a common point for all the columns. If you see 10 diodes on the PCB, connected together on one side - you found it. Diodes may be scattered around the PCB (as is in Harris case) - but then they all connect to a common bus **far from the driver**. Short out the diodes, connect common point to drivers'(which is in my case invariably is a 7442) ground pin.
* Connect common point of rows (it's close to 74145(s), look for a swarm of jumper wires between tracks that go alongside the row traces) to the ground lead of the 74145 you removed.
* Solder the wires that will go to the controller. I used ribbon cable with some janky totally-not-copper conductors, and it worked.
* It's probably beneficial to have separate rows and columns grounds. At the time of this writing I do not know, my scope is not good enough to determine that.<br>
**!!WARNING!!** PCBs on all 3 boards I've seen is made from unbelievably shitty phenolic paper and pads delaminate if you just look at them wrong, let alone touching. Be extra careful.
* If you want to use the lock LED - don't accidentally power the whole board. Find it's driver (Usually(?) 75452 with ganged outputs), **cut it's Vcc trace** and power it from the controller VDD or VDDIO. Don't power from ExpHdr pin - not only it won't work, you'll fry the pin.

# Preflight checks
Resistance between the ground wire and any column or row "at the controller end of the cables" must be 1.5 to 2.5Ω. It seems to be around 0.2Ω/switch, but my ohmmeter has 0.1Ω scale, so it's not very trustworthy.

If all rows have same resistance to the ground and it's around 580Ω - you have not grounded the rows common point (see above).

# Assembly
Most recommendations from capsense part still apply, except for pinout.

#### DO NOT touch columns. Not even to reorder.

Columns must be in this particular order, because it's basically the only way to lay out 4 analog MUXes on the chip.

Don't worry about column order either. Solder in a way that will make cabling easier. You'll fix it all in layout. Cortron with a numpad has batshit insane layout anyway, you won't make it worse.

Default pinout:

![Default pinout](docs/MagValve/pinout.jpg)

Note that you will need to desolder 3 capacitors if you don't remap the rows. It's not that hard - just use the wide tip and some flux. Rest the tip on the capacitor and just nudge it off the pad after a couple of seconds. Sometimes just lifting the tip will pull the capacitor away.

You _MIGHT_ want to add 100Ω resistors in series with "rows" and set row pin output mode to "Fast" - that _MIGHT_ prolong the part's life while making readouts a bit more stable at the expense of lower thresholds (should still be non-zero - probably somewhere in 2 to 5 range).

# First Startup

First run will almost certainly result in [UNSAFE] condition.
* Go to hardware settings, verify charge delay is 20+ and discharge delay is 3-4.
* Debouncing must be **at least** 4.
* Set thresholds to 15, apply values.
* Upload, COMMIT (Warning: takes several seconds), REVERT(essentially just a hard reset, you just wrote the config), reconnect.

This should allow scan module to start without tripping the breaker and at least some keys should register when pressed. If they don't - drop to 10 (don't forget to apply and upload). I never needed less than 10.

Thresholds in a 10-15 range should work. There can be a key that trips when others are pressed (for some reason it was spacebar in both of my cases). Either bump that key's threshold couple notches or increase debouncing interval.

# OK, so it doesn't work.
* You're trying to test NKRO by pressing 20 keys and there's key spam: [STOP IT!](https://www.youtube.com/watch?v=4BjKS1-vjPs)<br>
I'll have a separate doc with oscillograms which explains why that happens. But seriously. Stop it.
* Whole row doesn't register: Verify row resistance is in range. Verify that you removed capacitors pointed out on the pinout pic. Verify solder joint quality. The usual.

# Some insane shit is happening
* Some keys don't register, or only register when you touch the plate with your other hand.
* or register only if pressed FIRMLY, and even then may only intermittently register.
* or register only if pressed firmly AND then shifted left/right/up/down.
* or register only you put something **FOR THE LOVE OF GOD, NON-CONDUCTIVE** under one side of the PCB and press firmly.

Congratulations, you're in for the Total Rework.

## If you have a scope
This won't save you from the rework. But you can at least confirm if it worth doing. To figure that out, connect your scope to the row which has a problematic key, and set discharge delay to 10-15 (don't forget to move it back to ~4 after the experiment! Prolonged shorting of the GPIO to the ground is a surefire way to fry it!) and observe.

Let's look first at the good signal first:

![good][docs/good.png]

And now at bad:
![bad][docs/bad.png]

It's mostly explained in the pictures - but one tell-tale sign is left out: the post-impulse oscillations. The good signal drops to the ground quickly and stays there. The bad is much wigglier. If you see that - rework is your ticket. If not - I don't know what it is, you're on your own. I can speculate looking at your scope screenshots, but by the time you're reading it I won't have ANY cortron boards in my reach. 

## Total Rework
Leads on at least some of those switches are made of some strange material which refuses to wet by 37Pb63Sn solder.

How to tell: touch the solder joint with a soldering iron (give it some flux up front, because that thing was made 50 years ago, it has oxide CRUST on it, not "film") - and observe solder JUMPING away from the lead.

It takes copious amounts of RMA flux (I used DeoxIT RSF-R80-2) and a scraper tool to convince it to take some tin. The tool (and what happens after heating the joint) looks like this: https://www.instagram.com/p/B251_1UjHZ7/. It is made from a diode because that's what I had on hand. It doesn't have to be a diode - just some sturdy wire with a loop on it. Or a wire brush. Improvise.

Leads had to be bathed in flux and scraped for 10-15 seconds, repeatedly - with new dose of flux applied to the lead every round.

I am NOT joking about scraping.

Even with scraping, after washing away the flux residue I see lots of "cold joints" and even holes in the solder between lead and the pad - but it works, and it's not my keyboard so I'm not going to switch to OA flux (or even IA/tip tinner) and have a chance to corrode everything in a couple of years. RMA barely cuts it, even after 30 seconds of bathing in a continuous supply of it.

Unfortunately, I didn't measure resistance before the resoldering operation, so I don't know how large it was. If you encounter one of those boards - please don't forget to measure the resistances beforehand and drop me a note - by email or via PM on DT - or even in the topic.