#include "HEADER/HeaderArchiver.h"

/******************************************************************************************
*removeElement - функция удаляет файл с именем file_name из архива с именем archive_name.
*
*Возвращает ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE, если файла с именем fileName в архиве нет.
******************************************************************************************/


int removeElement(const char *file_name, const char *archive_name){
	FILE *helpArchive, *archive;
	char checksum[CHECKSUM_LENGTH + 1];
	char currentFileName[BLOCK_NAME],
		buffer[BLOCK];
	off64_t size, currentSize = 0, sizeRead;
	char sym;
	int lengthOfCurrentFileName = 0;
	int fileIsFound = 0;
	char helpFileName[BLOCK_NAME];
	extern current_endianness;

	tmpnam(helpFileName);
	shiftLeftStringOne(helpFileName);
	helpArchive = fopen(helpFileName, "wb");


	archive = fopen(archive_name, "rb");


	fread(checksum, 1, CHECKSUM_LENGTH + 1, archive);
	fwrite(CHECKSUM, 1, CHECKSUM_LENGTH, helpArchive);
	fputc('\0', helpArchive);
	

follow_file:
	while((sym = fgetc(archive)) && sym != END_OF_ARCHIVE){
		currentFileName[lengthOfCurrentFileName++] = sym;
	}
	if(sym == END_OF_ARCHIVE){ 
		fputc(END_OF_ARCHIVE, helpArchive);
		fclose(archive);
		fclose(helpArchive);
		remove(archive_name);
		rename(helpFileName, archive_name);
		return fileIsFound ? 0 : ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE;
	}

	currentFileName[lengthOfCurrentFileName] = '\0'; 
	if(strcmp(currentFileName, file_name)){
		fprintf(helpArchive, "%s", currentFileName);
		fputc('\0', helpArchive);
		if(currentFileName[strlen(currentFileName) - 1] == EMPTY_DIRECTORY)
		{
			currentSize = 0;
			lengthOfCurrentFileName = 0;
			goto follow_file;
		}

		fread(&size, 1, sizeof(off64_t), archive);
		fwrite(&size, 1, sizeof(off64_t), helpArchive);
		if(current_endianness == ENDIANNESS_LITTLE)
			flip(&size, sizeof(off64_t));

		while(currentSize + BLOCK <= size)
		{
			fread(buffer, sizeof (char), BLOCK, archive);
			fwrite(buffer, sizeof (char), BLOCK, helpArchive);
			currentSize += BLOCK;
		}

		fread(buffer, sizeof (char), sizeRead = size - currentSize, archive);
		fwrite(buffer, sizeof (char),sizeRead, helpArchive);
		currentSize = 0;
		lengthOfCurrentFileName = 0;
		goto follow_file;
	}
	if(currentFileName[strlen(currentFileName) - 1] == EMPTY_DIRECTORY)
		{
			
			fileIsFound = 1;
			lengthOfCurrentFileName = 0;
			goto follow_file;
		}
	fread(&size, 1, sizeof(off64_t), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&size, sizeof(off64_t));
	_fseeki64(archive, size, SEEK_CUR);
	fileIsFound = 1;
	lengthOfCurrentFileName = 0;
	goto follow_file;

}
