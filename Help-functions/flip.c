#include "../Huffman/HEADER/HeaderHuffman.h"

//Переворот массива не более чем из sizeof(unsigned __int64) байт

void flip(void *ptr, size_t size){
	char result[sizeof(unsigned __int64)];
	char *currentPtr = ptr;
	size_t i;

	for(i = 0; i < size; i++)
		result[i] = currentPtr[size-i-1];
	for(i = 0; i < size; i++)
		currentPtr[i] = result[i];
	ptr = currentPtr;
}

