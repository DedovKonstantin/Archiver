#include "../HEADER/HeaderHuffman.h"

/******************************************************************
*Метод обнуления size элементов в таблице символов symbol_table
******************************************************************/

void resetCounts(informationOfSymbol *symbol_table, int size){
	int i;

	for(i = 0; i < size; i++)
		symbol_table[i].count = 0;
}
