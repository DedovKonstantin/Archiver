#include "../HEADER/HeaderArchiver.h"

//Вычисление длины LPCWSTR-строки

int strlenLPWSTR(LPCWSTR szString){
	int result = 0;
	LPCWSTR ptr = szString;

	while(*ptr++)
		result++;
	return result;
}
