#include "../HEADER/HeaderArchiver.h"

//Преобразование wchar_t * в char *

void LPWSTRtoChar(const wchar_t *sz, char *result){
	int i, len = strlenLPWSTR(sz);
 
	for(i = 0; i <=len; i++)
		result[i] = (char)sz[i];
}
