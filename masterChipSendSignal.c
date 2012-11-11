//
// main.c
// HeadMaster
//
// Created by Kevan Ahlquist on 10/20/12.
// Copyright (c) 2012 Tesla Works. All rights reserved.
//

#include "DMXlib.h"
#include "I2C.h"
#include <xc.h>

// Global Variables

// Device Configuration


#pragma config OSC = HSPLL		//high speed/ PLL enabled oscilator mode
#pragma config FCMEN = OFF		//fail-safe clock moniter
#pragma config IESO = OFF		//inter/ext osc switchover (disabled)
#pragma config BOREN = OFF		//brown-out reset bits
#pragma config BORV = 3			//brown out voltage bit (min setting?)
#pragma config PWRT = OFF		
#pragma config WDT = OFF		//watch dog timer (off)
#pragma config WDTPS = 32768	//wdt post-scaler (don't care)
#pragma config CCP2MX = PORTC	//CCP2 mux bit: either PORTC or PORTBE
#pragma config PBADEN = OFF
#pragma config LPT1OSC = OFF
#pragma config MCLRE = ON		//MCLR pin enabled
#pragma config DEBUG = OFF
#pragma config STVREN = ON		//stack full/underflow will cause reset when ON
#pragma config LVP = OFF
#pragma config XINST = OFF

#pragma config CP0 = OFF		//all write protection bits, some missing, should be OFF
#pragma config CP1 = OFF
#pragma config CPB = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTRB = OFF


// Interrupts

__interrupt(high_priority) void HighPriorityInterrupt(void)
{
}

//
// Code
//

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
