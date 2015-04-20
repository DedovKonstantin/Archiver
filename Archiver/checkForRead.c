#include "HEADER/HeaderArchiver.h"

/**********************************************
*checkForRead - функция проверки архива на повреждение. 
*(проверка контрольных сумм)
*
*archive_name - имя архива.
*
*Взвращает 1, если архив повреждён, 0 - иначе.
**********************************************/


int checkForRead(const char *archive_name, const char operation){
	FILE *archive;
	char checksum[CHECKSUM_LENGTH + 1];
	struct __stat64 info;
	off64_t currentSize = 0;



	if((archive = fopen(archive_name, "rb")) == NULL){
		if(operation == INSERT)
			return 1;
		printf("Archive \"%s\" not found.\n", archive_name);
		return 0;
	}
	__stat64(archive_name,&info);
	if(info.st_size < CHECKSUM_LENGTH+1){
		printf("Archive \"%s\" corrupted.\n", archive_name);
		fclose(archive);
		return 0;
	}
	fread(checksum, 1, CHECKSUM_LENGTH+1, archive);
	if(strcmp(checksum, CHECKSUM)){
		printf("Archive \"%s\" corrupted.\n", archive_name);
		fclose(archive);
		return 0;
	}
	_fseeki64(archive, -1, SEEK_END);
	if(fgetc(archive) != END_OF_ARCHIVE){
		printf("Archive \"%s\" corrupted.\n", archive_name);
		fclose(archive);
		return 0;
	}
	return 1;
}
