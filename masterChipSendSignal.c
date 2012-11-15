// 
// masterChipSendSignal.c
// HeadMaster
// 
// Created by Kevan Ahlquist on 10/20/12.
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
#include "I2C.h"
#include "MiniVec.h"
#include <xc.h>

// Global Variables

// Device Configuration

/*
#pragma config CPUDIV = NOCLKDIV
#pragma config USBDIV = OFF
#pragma config FOSC = HS
#pragma config PLLEN = ON
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRTEN = OFF
#pragma config BOREN = OFF
#pragma config BORV = 30
#pragma config WDTEN = OFF
#pragma config WDTPS = 32768
#pragma config MCLRE = ON
#pragma config HFOFST = OFF
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config XINST = OFF
#pragma config BBSIZ = OFF
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CPB = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTRB = OFF
#pragma config DEBUG = OFF
 */

// Interrupts

__interrupt(high_priority) void HighPriorityInterrupt(void)
{
}

//
// Code
//

void mapDmxToServo(char *dmx, char numberToMap)
{
    MVRightShift(dmx, numberToMap, 1);
    MVAdd(dmx, numberToMap, 30);
}

void Setup(void)
{
    TRISBbits.RB4 = 1; //SDA
    TRISBbits.RB6 = 1; //SCL

    //set up MSSP for master mode
    SSPSTATbits.SMP = 1;
    SSPSTATbits.CKE = 1;

    SSPCON1 = 0b00001000;

    SSPCON2 = 0b00000000;

    //Baud = Fosc/(4*SSPADD+1) = ~114kHz when Fosc @ 12MHz
    SSPADD = 100;

    PIR1bits.SSPIF = 0;

    SSPCON1bits.SSPEN = 1;
}

void main(void)
{
    int receiver = 0;

    Setup();
    DMXSetup();
    while(1)
    {
		DMXReceive();
        for (receiver = 0; receiver < 4; ++receiver) {
            sendI2C(receiver);
        }
    }
}
