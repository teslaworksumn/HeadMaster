//
//  I2C.h
//  HeadMaster
//
//  Created by Kevan Ahlquist on 11/3/12.
//  Copyright (c) 2012 Tesla Works. MIT license.
//

// =============================================================================
// Defines
// =============================================================================

#define NUMBER_OF_SLAVES 4
#define BYTES_PER_SLAVE 8

// =============================================================================
// Global Variables
// =============================================================================

char buffer[NUMBER_OF_SLAVES * BYTES_PER_SLAVE] = {7}; //Assuming the DMX parsing code will output data to this array
char slaveAddresses[NUMBER_OF_SLAVES] = {40}; //TODO: Need to hardcode slave addresses in this array

// =============================================================================
// Functions
// =============================================================================

// Sends BYTES_PER_SLAVE bytes of data from the buffer via I2C to the receiver.
// Reciever 0: bytes 0 to BYTES_PER_SLAVE
// Reciever 1: bytes BYTES_PER_SLAVE to (2*BYTES_PER_SLAVE - 1)
// etc.
void sendI2C(int receiver);

// Adds a single byte of data to the send buffer, does not wait for send completion to return
void sendByte(char data);
