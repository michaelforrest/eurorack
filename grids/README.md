# Grids Hacks by Michael forrest

I've mentioned these changes in a couple of videos [Hacking Mutable Instruments Grids - Getting Started](https://youtu.be/Eex-iLuUdiw) and [Analog Drum Hacks - Leaving the Laptop Episode 7](https://youtu.be/9eOOSP5kDd0).

## What each knob does now

![Grids Panel](https://mutable-instruments.net/modules/grids/images/manual.png)

A: Unchanged, B: Unchanged

### C1 - "Song" selector
On the low end of this knob is a 4 on the floor kick pattern with some standard hihats and snare hits. A workhorse.

Continuing upwards switches between 12 patterns based on rhythms from my album [How To Live Forever](http://michaelforrestmusic.com/posts/how-to-live-forever?filter=releases). 

### C2 - Close/Open Hi-hat
Hi-hats (for reasons I am embarrassed to admit) should be connected to TRIG 2 (closed) and ACC 3 (open). Then when you turn knob C2 past 12 o'clock, it will send open hi-hats as part of the rhythm.


E1 and E3 = Normal drum density behaviour
### E2 -> TRIG 2 Density and ACC2 Clock Pulse Resolution
I send an even clock pulse from TRIG 2 that subdivides the input clock signal. If you try to use both a Snare on TRIG 2 and the clock pulse on ACC 2, it's a mess...

### D - Swing/Wonk control
This should normally be kept dead centre at 12 o'clock. You can push it right or left for some crude "swing". I pull trigs forward and backwards but they're always aligned to the 96 clock pulse grid so it's pretty rudimentary.
