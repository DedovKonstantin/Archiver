#include "../HEADER/HeaderHuffman.h"

//Побитовое И для BigInteger

BigInteger bitwiseAND(const BigInteger b1, const BigInteger b2){
	BigInteger result;

	result.first  = b1.first  & b2.first;
	result.second = b1.second & b2.second;
	result.third  = b1.third  & b2.third;
	result.fourth = b1.fourth & b2.fourth;

	return result;
} 
