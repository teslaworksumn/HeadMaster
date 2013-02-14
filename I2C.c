//
//  I2C.c
//  headmaster
//
//  Created by Kevan Ahlquist on 11/3/12.
//  Copyright (c) 2013 Tesla Works. MIT license.
//

#include "I2C.h"
#include <xc.h>

// =============================================================================
// Code
// =============================================================================

void I2CSend(char *buffer, int receiver)
{
    int i = 0;

    SSPCON2bits.SEN = 1;        // Send Start bit
    I2CWaitForTransmission();
    
    I2CSendByte(receiver);      // Send reciever address
    
    // Send data for that reciever
    for (i = 0; i < BYTES_PER_SLAVE; ++i) {
        I2CSendByte(buffer[BYTES_PER_SLAVE * receiver + i]);
    }

    SSPCON2bits.PEN = 1;        // Send Stop bit
    I2CWaitForTransmission();
}

void I2CSendByte(char data) 
{
    SSPBUF = data;
    I2CWaitForTransmission();
}

void I2CWaitForTransmission(void)
{
    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0; 
}
