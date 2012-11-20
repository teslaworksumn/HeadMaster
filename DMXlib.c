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

// Constants
enum {
    DMXWaitBreak, DMXGotBreak, DMXWaitForStart, DMXWaitForData, DMXDone
} DMXState;

// Variables
int DMXBytesReceived; //16-bit counter
char DMXInputBuffer; //used to read RCREG to clear error conditions

void DMXSetup(void)
{
    for (int i = 0; i < DMX_BUFFER_SIZE; i++) { //Clear the receive buffer
        DMXBuffer[i] = 0;
    }

    TRISCbits.TRISC7 = 1;	//Allow the EUSART RX to control pin RC7
    TRISCbits.TRISC6 = 1;	//Allow the EUSART RX to control pin RC6

    BAUDCONbits.BRG16 = 1; 	//Enable EUSART for 16-bit Asynchronous operation
    SPBRGH = 0;
	
    SPBRG = 31; 			//Baud rate is 250KHz for 32MHz Osc. freq.

    TXSTA = 0x04;			//Enable transmission and CLEAR high baud rate

    RCSTA = 0x90;			//Enable serial port and reception
}

void DMXReceive(void)
{
    while (DMXState != DMXDone) {
        switch (DMXState) {
            case DMXWaitBreak:
                if (RCSTAbits.FERR){            //Framing error
                    DMXState = DMXGotBreak;
                    break;
                } else {
                    if (RCSTAbits.OERR) {
            	        RCSTAbits.CREN = 0;		//Toggling CREN clears OERR flag
            	        RCSTAbits.CREN = 1;
            	    }
                }
                break;
            case DMXGotBreak:
                DMXInputBuffer = RCREG;         //Read the Receive buffer to clear FERR
                DMXState = DMXWaitForStart;
                break;
            case DMXWaitForStart:
                while (!PIR1bits.RCIF) ;        //Wait until a byte has been received
                if (RCSTAbits.FERR) {
                    DMXState = DMXGotBreak;
                    PIR1bits.RCIF = 0;          //Reset the received flag
			        break;
		        } else {
			        DMXInputBuffer = RCREG;     //Read the Receive buffer
		        }
                if (DMXInputBuffer != DMX_START_CODE) { //if current byte isn't START code, ignore the frame
                    DMXState = DMXWaitBreak;
                    break;
                }
                else {
                    DMXBytesReceived = 0;	        //initialize counter
                    DMXState = DMXWaitForData;
                    break;
                }
            case DMXWaitForData:
                if (RCSTAbits.FERR) {	        //If a new framing error is detected (error or short frame)
                    DMXState = DMXDone;	        // the rest of the frame is ignored and the function exits
        	        break;
    	        }
                while (!PIR1bits.RCIF) ;        //Wait until a byte is correctly received
    	        DMXBuffer[DMXBytesReceived++] = RCREG;
        	    if (DMXBytesReceived < DMX_BUFFER_SIZE) {
                    DMXState = DMXWaitForData;
                    break;
                } else {
                    DMXState = DMXDone;
                    break;
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
