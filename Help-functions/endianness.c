#include "../Huffman/HEADER/HeaderHuffman.h"

//Определение типа машины.

int endianness(void){
	unsigned __int16 check = 1;

	if(*((unsigned char *)&check) == 0)
		return ENDIANNESS_BIG;
	return ENDIANNESS_LITTLE;
}
