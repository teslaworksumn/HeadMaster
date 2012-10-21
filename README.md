Animatronic Barbershop Quartet Main Heads Controller
====================================================

What's going to happen:
* We are going to first try using inline ASM code to parse the DMX input(Branch: Add-inline-ASM-to-receive-DMX) with the I2C output code written in C
* If that doesn't work, we will attempt to use assembly for everything, both receiving DMX and sending I2C.
* Also, I (@tylrtrmbl) think that we could study the assembly DMX parsing and implement that in C too.

Visit the [wiki](https://github.com/teslaworksumn/HeadMaster/wiki) for information on DMX and other stuff.