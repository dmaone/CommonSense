# Kumara K552N controller swap - Ohmic matrix example

Capacitive scanner actually works with contact matrix. It needs dioded, because there's no anti-ghosting algorithms in CommonSense, but it works.


Important settings: 8-bit ADC, thresholds of 60 (actual levels will be around 80.).

Charge/discharge delays are set to 18, and debouncing is 0 - seems to be no chatter even at zero..