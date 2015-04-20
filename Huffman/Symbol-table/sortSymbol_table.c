#include "../HEADER/HeaderHuffman.h"

/******************************************************************************************************
*Сортировка методом Хоара таблицы символов symbol_table по пункту count - количеству вхождений в файл.
*Без потери порядка пунктов symbol - символа и size - размера символа.
******************************************************************************************************/

void sortSymbol_table(informationOfSymbol *symbol_table, int left, int right){
	int left1 = left;
	int right1 = right;
	off64_t p_i64;
	unsigned char p_u8;
	off64_t ref = symbol_table[(left+right)/2].count;
		
	do{
		while(symbol_table[left1].count < ref)left1++;
		while(symbol_table[right1].count > ref)right1--;
		if(left1<=right1){
			if(symbol_table[left1].count>symbol_table[right1].count){
				symbol_table[left1].count = (p_i64 = symbol_table[right1].count, symbol_table[right1].count = symbol_table[left1].count, p_i64);
				symbol_table[left1].symbol = (p_u8 = symbol_table[right1].symbol, symbol_table[right1].symbol = symbol_table[left1].symbol, p_u8);
				symbol_table[left1].size = (p_u8 = symbol_table[right1].size, symbol_table[right1].size = symbol_table[left1].size, p_u8);
				
			}
			left1++;
			right1--;
		}
	}while (left1 <= right1);
	
	if(left1 < right) sortSymbol_table(symbol_table, left1, right);
	if(left < right1) sortSymbol_table(symbol_table, left, right1);
}
