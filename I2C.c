//
//  I2C.c
//  headmaster
//
//  Created by Kevan Ahlquist on 11/3/12.
//  Copyright (c) 2012 Tesla Works. MIT license.
//

#include "I2C.h"
#include <xc.h>

// =============================================================================
// Code
// =============================================================================

void I2CSend(int receiver)
{
    int i = 0;

    SSPCON2bits.SEN = 1;
    while (!PIR1bits.SSPIF);
    I2CSendByte(slaveAddresses[receiver]);
    while (!PIR1bits.SSPIF);

    for (i = 0; i < BYTES_PER_SLAVE; ++i) {
        I2CSendByte(buffer[BYTES_PER_SLAVE * receiver + i]);
        while (!PIR1bits.SSPIF);
    }

    SSPCON2bits.PEN = 1;
    PIR1bits.SSPIF = 0;
}

void I2CSendByte(char data) {
    SSPBUF = data;
    PIR1bits.SSPIF = 0;
}
