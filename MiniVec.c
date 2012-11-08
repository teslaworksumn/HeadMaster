/* MiniVec - Really, really small library for performing uniform math on arrays
 * Organization: Tesla Works
 * Project: Animatronic heads
 *	Unroll all functions for maximum performance benefit
 */

#include "MiniVec.h"
#include <string.h>

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
