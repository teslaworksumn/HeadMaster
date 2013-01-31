//
//  HeadMaster.c
//  HeadMaster
//
//  Created by Kevan Ahlquist on 10/20/12.
//  Copyright (c) 2013 Tesla Works. MIT license.
//

#include "DMX.h"
#include "I2C.h"
#include "MiniVec.h"
#include <xc.h>

// =============================================================================
// Configuration Bits
// =============================================================================

#pragma config OSC = HSPLL      // High speed/ PLL enabled oscilator mode
#pragma config FCMEN = OFF      // Fail-safe clock monitor
#pragma config IESO = OFF       // Int/ext osc switchover (disabled)
#pragma config BOREN = OFF      // Brown-out reset bits disabled
#pragma config BORV = 3         // Brown-out voltage bit (min setting)
#pragma config PWRT = OFF       // Power-up timer
#pragma config WDT = OFF        // Watch dog timer (off)
#pragma config WDTPS = 32768    // WDT post-scaler (don't care)
#pragma config PBADEN = OFF     // PortB A/D Enable bit
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator
#pragma config MCLRE = ON       // MCLR pin enabled
#pragma config STVREN = ON      // Stack full/underflow will cause reset when ON
#pragma config BBSIZ = 1024     // Boot Block size (1K words/2K bytes)
#pragma config LVP = OFF        // Single-supply ICSP Enable bit
#pragma config XINST = OFF      // Extended Instruction Set Enable bit

// All write protection bits, should be OFF
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config CP4 = OFF
#pragma config CP5 = OFF
#pragma config CPD = OFF
#pragma config CPB = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
#pragma config WRT4 = OFF
#pragma config WRT5 = OFF
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config WRTD = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF
#pragma config EBTR4 = OFF
#pragma config EBTR5 = OFF
#pragma config EBTRB = OFF

// =============================================================================
// Defines
// =============================================================================

#define SERVO_BIT_OFFSET 1
#define SERVO_VALUE_OFFSET 30
#define DMX_START_CHANNEL 0
#define DMX_BUFFER_SIZE (NUMBER_OF_SLAVES * BYTES_PER_SLAVE)

// =============================================================================
// Code
// =============================================================================

int HMReadDMXStartChannel(void);
void HMMapDmxToServo(char *dmx, char numberToMap);
void HMSetup(DMXDevice *dmxDevice, char *dmxBuffer);

__interrupt(high_priority) void HighPriorityInterrupt(void)
{
}

int HMReadDMXStartChannel(void)
{
    int dmxAddress = 0;
    
    if (!PORTAbits.RA5) {
        dmxAddress += 16;
    }
    if (!PORTAbits.RA3) {
        dmxAddress += 32;
    }
    if (!PORTAbits.RA2) {
        dmxAddress += 64;
    }
    if (!PORTAbits.RA1) {
        dmxAddress += 128;
    }
    if (!PORTAbits.RA0) {
        dmxAddress += 256;
    }

    return dmxAddress;
}

void HMMapDmxToServo(char *dmx, char numberToMap)
{
    MVRightShift(dmx, numberToMap, SERVO_BIT_OFFSET);
    MVAdd(dmx, numberToMap, SERVO_VALUE_OFFSET);
}

void HMSetup(DMXDevice *dmxDevice, char *dmxBuffer)
{
    TRISBbits.RB4 = 1; // SDA
    TRISBbits.RB6 = 1; // SCL

    // Set up MSSP for master mode
    SSPSTATbits.SMP = 1;
    SSPSTATbits.CKE = 1;

    SSPCON1 = 0b00001000;
    SSPCON2 = 0b00000000;

    // Baud = Fosc / (4 * (SSPADD + 1)) = 100kHz w/ Fosc = 40MHz
    SSPADD = 99;

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

    HMSetup(&dmxDevice, dmxBuffer);

    while(1)
    {
        DMXReceive(&dmxDevice);
        HMMapDmxToServo(dmxDevice.buffer, dmxDevice.bufferSize);
        for (receiver = 0; receiver < NUMBER_OF_SLAVES; ++receiver) {
            I2CSend(receiver);
        }
    }
}
