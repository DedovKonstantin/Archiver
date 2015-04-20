#include "../HEADER/HeaderHuffman.h"

unsigned __int16 maskPositionOne(const int pos){
	unsigned __int16 result = 1;

	result <<= 8*sizeof(unsigned __int16) - pos;
	return result;
}
