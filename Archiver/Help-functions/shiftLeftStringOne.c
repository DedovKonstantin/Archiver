#include "../HEADER/HeaderArchiver.h"

//сдвиг статического массива символов s на позицию влево

void shiftLeftStringOne(char *s){
	int i, len = strlen(s);

	for(i = 0; i < len; i++)
		s[i] = s[i+1];
}
