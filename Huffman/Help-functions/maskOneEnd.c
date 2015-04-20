#include "../HEADER/HeaderHuffman.h"

/******************************************************************************
*maskOneEnd - возвращает число типа unsigned __int16,
*	в двоичной записи которого все биты нули, кроме последних size (единиц).
*Например, maskOneEnd(3) == 0000 0000 0000 0111
******************************************************************************/

unsigned __int16 maskOneEnd(const int size){
	unsigned __int16 result = 1, i;

	for(i = 0; i < size; i++)
		result *= 2;
	result--;
	return result;
}

