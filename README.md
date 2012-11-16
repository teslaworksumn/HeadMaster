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

* We are going to first try using inline ASM code to parse the DMX input with the I2C output code written in C.
* If that doesn't work, we will attempt to use assembly for everything, both receiving DMX and sending I2C.
* Also, I (@tylrtrmbl) think that we could study the assembly DMX parsing and implement that in C too.

Visit the [wiki] for information on DMX and other stuff.

Compiling
---------

* It is recommended that you use the MPLAB X IDE for development
  * Use version 1.41 or later
  * Do not include MPLAB X project files when committing! Instead, keep a separate project and then "Add Existing Files", the ones that are part of this project. For now, our repository will be source code only.
* This project uses the XC8 compiler in CCI syntax mode
  * Use version 1.10 or later
  * To enable CCI:
    1. Right-click your project and select "Properties"
    2. Go to __XC8 Global Options -> XC8 Compiler__
    3. Check the __Use CCI Syntax__ check box

License
-------

This project is released under the MIT License. See `LICENSE`.  
Copyright (c) 2012 Tesla Works.

[wiki]: https://github.com/teslaworksumn/HeadMaster/wiki
