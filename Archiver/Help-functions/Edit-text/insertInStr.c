#include "../../HEADER/HeaderArchiver.h"

/*****************************************************************
Вставка символа c в статическую строку s в позицию position
*****************************************************************/

void insertInStr(char *s, size_t position, char c)
{
	__int64 i;

	for(i = strlen(s)+1; i >= position; i--)
		s[i] = i == position ? c : s[i-1];
}
