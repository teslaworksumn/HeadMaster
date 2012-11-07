/*
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		Functions for use with PIC18 microcontrollers to receive DMX data
 *
 * Author: Kevan Ahlquist (@aterlumen)
 */
 
 // Variables
const int DMXBufferSize = 512;
char DMXBuffer[DMXBufferSize];


// Functions
void DMXSetup(void);
void DMXReceive(void);
