//
//  DMX.c
//  HeadMaster
//
//  Created by Kevan Ahlquist on 11/8/12.
//  Copyright (c) 2012 Tesla Works. MIT license.
//

#include "DMX.h"
#include <xc.h>

// =============================================================================
// Types
// =============================================================================

typedef enum _DMXState {
    DMXWaitBreak,
    DMXGotBreak,
    DMXWaitForStart,
    DMXWaitForData,
    DMXDone
} DMXState;

// =============================================================================
// Code
// =============================================================================

void DMXSetup(void);
void DMXReceive(DMXDevice *device);

void DMXSetup(void)
{
    TRISCbits.TRISC7 = 1;       // Allow the EUSART RX to control pin RC7
    TRISCbits.TRISC6 = 1;       // Allow the EUSART TX to control pin RC6
    BAUDCONbits.BRG16 = 1;      // Enable EUSART for 16-bit asynchronos operation
    TXSTA = 0x04;               // Enable transmission and CLEAR high baud rate
    RCSTA = 0x90;               // Enable serial port and reception
    
    // Set up serial port baud rate of 250 kHz (DMX) with 40 MHz Fosc
    SPBRGH = 0;
    SPBRG = 39;
}

void DMXReceive(DMXDevice *device)
{
    DMXState state = DMXWaitBreak;
    int startCounter = 0;
    int bufferIndex = 0;
    char tmp;

    // Cache device characteristics for performance
    // Only FSR is available for pointers. We cache these so we can reserve the pointer FSR for using dmxBuffer
    char *dmxBuffer = device->buffer;
    int startChannel = device->startChannel;
    int bufferSize = device->bufferSize;

    while (state != DMXDone) {
        switch (state) {
            case DMXWaitBreak:
                if (RCSTAbits.FERR){            // Framing error
                    state = DMXGotBreak;
                    break;
                } else if (RCSTAbits.OERR) {
                    RCSTAbits.CREN = 0;         // Toggling CREN clears OERR flag
                    RCSTAbits.CREN = 1;
                }
                break;
            
            case DMXGotBreak:
                tmp = RCREG;                    // Read the receive buffer to clear FERR
                state = DMXWaitForStart;
                break;
            
            case DMXWaitForStart:
                while (!PIR1bits.RCIF) ;        // Wait until a byte has been received
                if (RCSTAbits.FERR) {
                    state = DMXGotBreak;
                    break;
                } else {
                    tmp = RCREG;                // Read the receive buffer
                }
                
                if (tmp != DMX_START_CODE) {    // If current byte isn't START code, ignore the frame
                    state = DMXWaitBreak;
                    break;
                } else {
                    startCounter = 0;            // Initialize counter
                    bufferIndex = 0;
                    state = DMXWaitForData;
                    break;
                }
            
            case DMXWaitForData:
                // If a new framing error is detected (error or short frame) the rest of the frame
                // is ignored and a new synchronization is attempted
                if (RCSTAbits.FERR) {
                    state = DMXWaitBreak;
                    break;
                }
                if (PIR1bits.RCIF) {            // Wait until a byte is correctly received
                    if (startCounter < startChannel) {
                        tmp = RCREG;            // Clear RCIF
                        startCounter++;
                    } else if (bufferIndex < bufferSize) {
                        dmxBuffer[bufferIndex] = RCREG;
                        bufferIndex++;
                    } else {
                        state = DMXDone;
                    }
                }
                break;
            
            case DMXDone:
                return;
            
            default:
                // You're in no man's land now
                break;
        }
    }
}
