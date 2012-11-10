/*
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		Functions for use with PIC18 microcontrollers to receive DMX data
 *
 * Author: Kevan Ahlquist (@aterlumen)
 */

// Defines
#define DMX_BUFFER_SIZE 512
#define DMX_START_CODE 0x00
 
 // Variables
char DMXBuffer[DMX_BUFFER_SIZE];

// Functions
void DMXSetup(void);
void DMXReceive(void);
