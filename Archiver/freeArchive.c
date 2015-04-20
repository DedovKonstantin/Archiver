#include "HEADER/HeaderArchiver.h"

/************************************************************************
*freeArchive - функция освобождает архив от файлов (без распаковки)
*
*archive_name - имя архива.
*
*Возвращает ERROR_ARCHIVE_IS_NOT_WRITE, если архив закрыт на запись.
************************************************************************/
int freeArchive(const char *archive_name){
	FILE *archive;

	if((archive = fopen(archive_name, "wb")) == NULL)
		return ERROR_ARCHIVE_IS_NOT_WRITE;
	fwrite(CHECKSUM, 1, CHECKSUM_LENGTH, archive);
	fputc('\0', archive);
	fputc(END_OF_ARCHIVE, archive);

	fclose(archive);
	return 0;
}
