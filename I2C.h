//
//  I2C.h
//  HeadMaster
//
//  Created by Kevan Ahlquist on 11/3/12.
//  Copyright (c) 2013 Tesla Works. MIT license.
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
char slaveAddresses[NUMBER_OF_SLAVES] = {0, 1, 2, 3};

// =============================================================================
// Functions
// =============================================================================


void I2CSend(int receiver, char *buffer);

void I2CSendByte(char data);
