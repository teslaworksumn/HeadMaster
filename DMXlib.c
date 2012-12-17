// 
// DMXlib.c
// HeadMaster
// 
// Created by Kevan Ahlquist on 11/8/12.
// Copyright (c) 2012 Tesla Works.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#include "DMXlib.h"
#include <xc.h>

// Types

typedef enum _DMXState {
    DMXWaitBreak,
    DMXGotBreak,
    DMXWaitForStart,
    DMXWaitForData,
    DMXDone
} DMXState;

// Code

void DMXSetup(void)
{
    TRISCbits.TRISC7 = 1;	//Allow the EUSART RX to control pin RC7
    TRISCbits.TRISC6 = 1;	//Allow the EUSART RX to control pin RC6

    BAUDCONbits.BRG16 = 1; 	//Enable EUSART for 16-bit Asynchronous operation
    SPBRGH = 0;
	
    SPBRG = 31; 			//Baud rate is 250KHz for 32MHz Osc. freq.

    TXSTA = 0x04;			//Enable transmission and CLEAR high baud rate

    RCSTA = 0x90;			//Enable serial port and reception
}

void DMXReceive(DMXDevice *device)
{
    DMXState state = DMXWaitBreak;
    int startCounter = 0;
    int bufferIndex = 0;
    char tmp;

    // Cache device characteristics for performance
    // Only FSR is available for pointers. We cache these so we can reserve the pointer FSR for using dmxBuffer.
    char *dmxBuffer = device->buffer;
    int startChannel = device->startChannel;
    int bufferSize = device->bufferSize;

    while (state != DMXDone) {
        switch (state) {
            case DMXWaitBreak:
                if (RCSTAbits.FERR){            //Framing error
                    state = DMXGotBreak;
                    break;
                } else {
                    if (RCSTAbits.OERR) {
            	        RCSTAbits.CREN = 0;		//Toggling CREN clears OERR flag
            	        RCSTAbits.CREN = 1;
            	    }
                }
                break;
            case DMXGotBreak:
                tmp = RCREG;                    //Read the Receive buffer to clear FERR
                state = DMXWaitForStart;
                break;
            case DMXWaitForStart:
                while (!PIR1bits.RCIF) ;        //Wait until a byte has been received
                if (RCSTAbits.FERR) {
                    state = DMXGotBreak;
			        break;
		        } else {
			        tmp = RCREG;                //Read the Receive buffer
		        }
                if (tmp != DMX_START_CODE) {    //if current byte isn't START code, ignore the frame
                    state = DMXWaitBreak;
                    break;
                }
                else {
                    startCounter = 0;	        //initialize counter
                    bufferIndex = 0;
                    state = DMXWaitForData;
                    break;
                }
            case DMXWaitForData:
                if (RCSTAbits.FERR) {	        //If a new framing error is detected (error or short frame)
                    state = DMXWaitBreak;	// the rest of the frame is ignored and a new synchronization
        	        break;                      //is attempted
    	        }
                if (PIR1bits.RCIF) {	        //Wait until a byte is correctly received
                    if (startCounter < startChannel) {
                        tmp = RCREG;            // Clear RCIF;
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
                // You're in no man's land now.
                break;
        }
    }
}
