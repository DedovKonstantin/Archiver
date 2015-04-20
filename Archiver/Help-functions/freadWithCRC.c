#include "../HEADER/HeaderArchiver.h"

/*******************************************************
*Чтение из файла + подсчёт CRC
*******************************************************/

void freadWithCRC(void *ptr, size_t size, FILE *f, unsigned __int64 *CRC_v){
	fread(ptr, 1, size, f);
	*CRC_v += getCheckSumString(ptr, size);
}
