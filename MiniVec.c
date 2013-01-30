//
//  MiniVec.c
//  HeadMaster
//
//  Created by Taylor Trimble on 11/11/12.
//  Copyright (c) 2012 Tesla Works. MIT license.
//

#include "MiniVec.h"
#include <string.h>

// =============================================================================
// Functions
// =============================================================================

void MVSet(char vector[], char vectorCount, char value)
{
	memset(vector, value, vectorCount);
}

void MVAdd(char vector[], char vectorCount, char numberToAdd)
{
    int index;
    for (index = 0; index < vectorCount; ++index) {
	   vector[index] += numberToAdd;
    }
}

void MVRightShift(char vector[], char vectorCount, char bitsToShift)
{
    int index;
    for (index = 0; index < vectorCount; ++index) {
	   vector[index] = vector[index] >> bitsToShift;
    }
}
