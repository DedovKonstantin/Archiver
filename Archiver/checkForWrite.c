#include "HEADER/HeaderArchiver.h"

/*******************************************************************
*checkForWrite - функция проверки архива на повреждение и записи.
*(проверка контрольных сумм)
*
*archive_name - имя архива.
*
*Взвращает 1, если архив повреждён, 0 - иначе.
*******************************************************************/


int checkForWrite(const char *archive_name, const char operation){
	FILE *archive;
	char checksum[CHECKSUM_LENGTH + 1];
	char currentSymbols[BLOCK];
	off64_t size, sizeRead, currentSize = 0, currentSizeOfArchive = 0, sizeOfMeta;
	char sym;
	int lengthOfCurrentFileName = 0;
	char currentFileName[BLOCK_NAME];
	unsigned __int64 CRC_v, CRC_current_v = 0;
	struct __stat64 info;
	union meta current_meta;
	extern current_endianness;


	if((archive = fopen(archive_name, "rb")) == NULL){
		if(operation == INSERT)
			return 1;
		printf("Archive \"%s\" not found.\n", archive_name);
		return 0;
	}
	__stat64(archive_name,&info);
	_fseeki64(archive, 0, 0);
	if(info.st_size < CHECKSUM_LENGTH+1){
		printf("Archive \"%s\" corrupted.\n", archive_name);
		fclose(archive);
		return 0;
	}
	if((currentSizeOfArchive += CHECKSUM_LENGTH+1) > info.st_size)
		goto archive_corrupted_gt;
	fread(checksum, 1, CHECKSUM_LENGTH+1, archive);
	if(strcmp(checksum, CHECKSUM)){
archive_corrupted_gt:
		printf("Archive \"%s\" corrupted.\n", archive_name);
		fclose(archive);
		return 0;
	}
	
follow_file:
	while((++currentSizeOfArchive <= info.st_size)&&(sym = fgetc(archive)) && sym != END_OF_ARCHIVE){
		currentFileName[lengthOfCurrentFileName++] = sym;
	}
	if(sym == END_OF_ARCHIVE){
		fclose(archive);
		return 1;
	}
	currentFileName[lengthOfCurrentFileName] = '\0'; 
	if(currentFileName[lengthOfCurrentFileName-1] == EMPTY_DIRECTORY)
	{
		lengthOfCurrentFileName = 0;

		goto follow_file;
	}


	if((currentSizeOfArchive+= sizeof(off64_t)) > info.st_size)
		goto archive_corrupted_gt;

	freadWithCRC(&current_meta.size_archive, sizeof(off64_t), archive, &CRC_current_v);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&current_meta.size_archive, sizeof(off64_t));


	size = current_meta.size_archive;
	if((currentSizeOfArchive+= size) > info.st_size || size < 0)
		goto archive_corrupted_gt;
	
	freadWithCRC(&current_meta.isZip,		sizeof(char),	archive, &CRC_current_v);
	freadWithCRC(&current_meta.size_file,	sizeof(off64_t),	archive, &CRC_current_v);
	freadWithCRC(&current_meta.ctime_v,		sizeof(time_t), archive, &CRC_current_v);
	freadWithCRC(&current_meta.attributes,	sizeof(DWORD),	archive, &CRC_current_v);




	sizeOfMeta = sizeof(char)//флаг сжатия
				+sizeof(off64_t)//размер исходного файла
				+sizeof(time_t)//дата создания файла
				+sizeof(DWORD)//attributes of file
				+sizeof(unsigned __int64);//CRC

	
	while(currentSize + BLOCK <= size - sizeOfMeta)
	{
		freadWithCRC(currentSymbols, BLOCK, archive, &CRC_current_v);
		currentSize += BLOCK;
	}
	freadWithCRC(currentSymbols, sizeRead = size - sizeOfMeta - currentSize, archive, &CRC_current_v);
	fread(&CRC_v, 1, sizeof(unsigned __int64), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&CRC_v, sizeof(unsigned __int64));
	
	if(CRC_current_v != CRC_v){
		printf("File \"%s\" corrupted.\n", currentFileName);
		fclose(archive);
		return 0;
	}
	lengthOfCurrentFileName = CRC_current_v = currentSize = 0;
	goto follow_file;


}
