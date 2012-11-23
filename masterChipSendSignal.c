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

// Device Configuration

#pragma config OSC = HSPLL      // High speed/ PLL enabled oscilator mode
#pragma config FCMEN = OFF      // Fail-safe clock moniter
#pragma config IESO = OFF       // Int/ext osc switchover (disabled)
#pragma config BOREN = OFF      // Brown-out reset bits disabled
#pragma config BORV = 3         // Brown out voltage bit (min setting)
#pragma config PWRT = OFF       // Power-up timer
#pragma config WDT = OFF        // Watch dog timer (off)
#pragma config WDTPS = 32768    // WDT post-scaler (don't care)
//#pragma config CCP2MX = PORTC   // CCP2 mux bit: either PORTC or PORTBE
#pragma config PBADEN = OFF     // PortB A/D Enable bit
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator
#pragma config MCLRE = ON       // MCLR pin enabled
#pragma config STVREN = ON      // Stack full/underflow will cause reset when ON
#pragma config LVP = OFF        // Single-supply ICSP Enable bit
#pragma config XINST = OFF      // Extended Instruction Set Enable bit

// All write protection bits, should be OFF
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config CPD = OFF
#pragma config CPB = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config WRTD = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF
#pragma config EBTRB = OFF

// Defines

#define SERVO_BIT_OFFSET 1
#define SERVO_VALUE_OFFSET 30
#define DMX_START_CHANNEL 0
#define DMX_BUFFER_SIZE (NUMBER_OF_SLAVES * BYTES_PER_SLAVE)

// Interrupts

__interrupt(high_priority) void HighPriorityInterrupt(void)
{
}

//
// Code
//

void mapDmxToServo(char *dmx, char numberToMap)
{
    MVRightShift(dmx, numberToMap, SERVO_BIT_OFFSET);
    MVAdd(dmx, numberToMap, SERVO_VALUE_OFFSET);
}

void Setup(DMXDevice *dmxDevice, char *dmxBuffer)
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

    // Configure DMX
    DMXSetup();
    dmxDevice->buffer = dmxBuffer;
    dmxDevice->startChannel = DMX_START_CHANNEL;
    dmxDevice->bufferSize = DMX_BUFFER_SIZE;
    MVSet(dmxDevice->buffer, DMX_BUFFER_SIZE, 0);
}

void main(void)
{
    DMXDevice dmxDevice;
    char dmxBuffer[DMX_BUFFER_SIZE];
    int receiver = 0;

    Setup(&dmxDevice, dmxBuffer);

    while(1)
    {
		DMXReceive(&dmxDevice);
        for (receiver = 0; receiver < NUMBER_OF_SLAVES; ++receiver) {
            sendI2C(receiver);
        }
    }
}
