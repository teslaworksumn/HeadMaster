// 
// I2C.h
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

// Constants
#define NUMBER_OF_SLAVES 4
#define BYTES_PER_SLAVE 8

char buffer[NUMBER_OF_SLAVES * BYTES_PER_SLAVE] = {7}; //Assuming the DMX parsing code will output data to this array
char slaveAddresses[NUMBER_OF_SLAVES] = {40}; //TODO: Need to hardcode slave addresses in this array

// Sends BYTES_PER_SLAVE bytes of data from the buffer via I2C to the receiver.
// Reciever 0: bytes 0 to BYTES_PER_SLAVE
// Reciever 1: bytes BYTES_PER_SLAVE to (2*BYTES_PER_SLAVE - 1)
// etc.
void sendI2C(int receiver);

// Adds a single byte of data to the send buffer, does not wait for send completion to return
void sendByte(char data);
