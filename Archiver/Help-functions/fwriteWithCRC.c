#include "../HEADER/HeaderArchiver.h"

/*******************************************************
*Запись в файл + подсчёт CRC
*******************************************************/

void fwriteWithCRC(const void *ptr, size_t size, FILE *f, unsigned __int64 *CRC_v){
	fwrite(ptr, 1, size, f);
	*CRC_v += getCheckSumString(ptr, size);
}
