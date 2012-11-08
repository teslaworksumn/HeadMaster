/*
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		Functions for use with PIC18 microcontrollers to receive DMX data
 *
 * Author: Kevan Ahlquist (@aterlumen)
 */
 
 // Constants
 const int DMXBufferSize = 512;
 const int DMXStartCode = 0x00;
 
 // Variables
char DMXBuffer[DMXBufferSize];


// Functions
void DMXSetup(void);
void DMXReceive(void);
