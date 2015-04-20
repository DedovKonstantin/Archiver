#include "../HEADER/HeaderArchiver.h"

//Вычисление CRC. Многочлен 2473*x^4 + 14083*x^3 + 52489*x^2 + 99991*x

unsigned __int64 getCheckSumString(const void *ptr, size_t size){
	unsigned __int64 result = 0;
	unsigned i;
	const unsigned char *s = ptr;

	for(i = 0; i < size; i++){
		if(i % 4 == 3)
			result += 2473*s[i]*s[i]*s[i]*s[i];
		if(i % 4 == 0)
			result += 14083*s[i]*s[i]*s[i];
		if(i % 4 == 1)
			result += 52489*s[i]*s[i];
		if(i % 4 == 2)
			result += 99991*s[i];

	}

	return result;
}
