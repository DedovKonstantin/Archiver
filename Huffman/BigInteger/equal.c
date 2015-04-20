#include "../HEADER/HeaderHuffman.h"

//Сравнение BigInteger
//Функция возвращает 1, если b1 == b2, 0 - иначе

int equal(BigInteger b1, BigInteger b2){
	if(
		b1.first == b2.first &&
		b1.second == b2.second &&
		b1.third == b2.third &&
		b1.fourth == b2.fourth
	)
		return 1;

	return 0;
}
