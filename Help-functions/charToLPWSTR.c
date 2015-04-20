#include "../Huffman/HEADER/HeaderHuffman.h"

//Преобразование char * в wchar_t*

void charToLPWSTR(const char *s, wchar_t *szString){
	int i, len = strlen(s);
 
	for(i = 0; i <=len; i++)
		szString[i] = (wchar_t)s[i];
}
