//
// I2C.h
// HeadMaster
//
// Created by Kevan Ahlquist on 10/20/12.
// Copyright (c) 2012 Tesla Works. All rights reserved.
//

// Sends 10 bytes of data from the buffer via I2C to the receiver.
// Reciever 0: bytes 0-9
// Reciever 1: bytes 10-19
// etc.
void sendI2C(int receiver);

// Adds a single byte of data to the send buffer, does not wait for send completion to return
void sendByte(char data);
