/* MiniVec - Really, really small library for performing uniform math on arrays
 * Organization: Tesla Works
 * Project: Animatronic heads
 *	Unroll all functions for maximum performance benefit
 */

void MVAdd(char vector[], int vectorCount, signed char numberToAdd)
{
    int index;
    for (index = 0; index < vectorCount; ++index) {
	vector[index] += numberToAdd;
    }
}

void MVRightShift(char vector[], int vectorCount, int bitsToShift)
{
    int index;
    for (index = 0; index < vectorCount; ++index) {
	vector[index] >> bitsToShift;
    }
}
