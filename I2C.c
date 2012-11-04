//
// I2C.c
// HeadMaster
//
// Created by Kevan Ahlquist on 10/20/12.
// Copyright (c) 2012 Tesla Works. All rights reserved.
//

#include "I2C.h"
#include <xc.h>

// Gloal Variables

char buffer[40] = {7}; //Assuming the DMX parsing code will output data to this array
char slaveAddresses[4] = {40}; //TODO: Need to hardcode slave addresses in this array

// Functions

void sendI2C(int receiver)
{
    int i = 0;

    SSPCON2bits.SEN = 1;
    while (!PIR1bits.SSPIF);
    sendByte(slaveAddresses[receiver]);
    while (!PIR1bits.SSPIF);

    for (i = 0; i < 10; ++i) {
	sendByte(buffer[10 * receiver + i]);
	while (!PIR1bits.SSPIF);
    }

    SSPCON2bits.PEN = 1;
    PIR1bits.SSPIF = 0;
}

void sendByte(char data) {
    SSPBUF = data;
    PIR1bits.SSPIF = 0;
}
