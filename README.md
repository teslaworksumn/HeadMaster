Animatronic Barbershop Quartet Main Heads Controller
====================================================

Grand Scheme Of Things
----------------------

This repository will contain the code for our master microcontroller for the Animatronic Barbershop Quartet. Its purpose is to:

* Receive DMX values via the USART
* Map the DMX values to values understood by the PWM slave chips
* Write the interpreted values to the PWM slave chips via I2C

There will be __4__ slave PWM chips. We think it will be possible to have *no slave microcontrollers*, only I2C slave PWM chips.  
For more information about these chips, visit the [wiki].

Roadmap
-------

What's going to happen:

* We are going to first try using inline assembly for parsing our DMX
* If that doesn't work, we'll attempt to use inline assembly for our I2C control (some simple modifications)
* Also, I (@tylrtrmbl) think that we could study the assembly DMX parsing and implement that in C too

Visit the [wiki] for information on DMX and other stuff.

[wiki]: (https://github.com/teslaworksumn/HeadMaster/wiki)