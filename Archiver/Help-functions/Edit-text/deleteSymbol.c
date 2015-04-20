#include "../../HEADER/HeaderArchiver.h"

/***************************************************
Удаление символа с номером position из строки s
***************************************************/

void deleteSymbol(char *s, size_t position)
{
	size_t i, len = strlen(s);

	for(i = position; i < len; i++)
		s[i] = s[i+1];
}
