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

    SSPCON2bits.SEN = 1;

    I2CSendByte(receiver);
    waitForSPIF();

    for (i = 0; i < BYTES_PER_SLAVE; ++i) {
        I2CSendByte(buffer[BYTES_PER_SLAVE * receiver + i]);
        waitForSPIF();
    }

    SSPCON2bits.PEN = 1;
    waitForSPIF();
}

void I2CSendByte(char data) 
{
    SSPBUF = data;

}

void waitForSPIF(void)
{
    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0; 
}
