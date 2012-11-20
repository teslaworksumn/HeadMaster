// 
// I2C.c
// HeadMaster
// 
// Created by Kevan Ahlquist on 11/3/12.
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
