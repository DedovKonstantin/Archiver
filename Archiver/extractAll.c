#include "HEADER/HeaderArchiver.h"

/********************************************************************************************
*extractAll - функция извлекает все файлы из архива (без удаления).
*
*archive_name - имя архива.
*
*Возвращает ERROR_MEMORY_DOES_NOT_ALLOCATE при нехватке динамической памяти.
*			ERROR_INCORRECT_CHECKSUM, если при извлечении обнаружился повреждённый файл.
********************************************************************************************/

int extractAll(const char *archive_name){
	FILE *archive;
	char checksum[CHECKSUM_LENGTH + 1];
	char currentFileName[BLOCK_NAME];
	char sym;
	int lengthOfCurrentFileName = 0;
	off64_t size;
	char isZip;
	int return_error;
	char helpFileName[BLOCK_NAME];
	char newFileName[BLOCK_NAME];
	wchar_t helpFileNameLPWSTR[BLOCK_NAME];
	extern current_endianness;


	archive	  = fopen(archive_name, "rb");
	fread(checksum, 1, CHECKSUM_LENGTH+1, archive);

	tmpnam(helpFileName);
	shiftLeftStringOne(helpFileName);
	fclose(fopen(helpFileName, "wb"));
follow_file:
	
	while((sym = fgetc(archive)) && sym != END_OF_ARCHIVE){
		currentFileName[lengthOfCurrentFileName++] = sym;
	}
	currentFileName[lengthOfCurrentFileName] = '\0';
	if(sym != END_OF_ARCHIVE){
		currentFileName[lengthOfCurrentFileName] = '\0';

		if(return_error = extract(currentFileName, archive_name, &isZip, newFileName)){
			if(return_error == SKIP_FILE_EXTRACT){
				printf("File \"%s\" skipped.\n", newFileName);
				goto skip_file_gt;
			}
			if(return_error == ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE){
				printf("File \"%s\" not found in archive.\n", currentFileName);
				goto skip_file_gt;
			}
			if(return_error == ERROR_INCORRECT_CHECKSUM){
				printf("File \"%s\" corrupted.\n", currentFileName);
				remove(helpFileName);
				return ERROR_INCORRECT_CHECKSUM;
			}


		}
		
		if(isZip){
			if((Huffman(OPERATION_UNZIP, helpFileName, newFileName, &isZip)) == ERROR_MEMORY_DOES_NOT_ALLOCATE)
				return ERROR_MEMORY_DOES_NOT_ALLOCATE;

			charToLPWSTR(newFileName, helpFileNameLPWSTR);
			SetFileAttributesW(helpFileNameLPWSTR, STANDART_ATTRIBUTES);
			remove(newFileName);
			rename(helpFileName, newFileName);
		}
		if(return_error != REPLACE_FILE_EXTRACT)
			printf("File \"%s\" unzipped.\n", currentFileName);
		else
			printf("File \"%s\" unzipped and replaced.\n", currentFileName);

skip_file_gt:
		if(currentFileName[strlen(currentFileName) - 1] == EMPTY_DIRECTORY)
		{
			lengthOfCurrentFileName = 0;
			goto follow_file;
		}
		fread(&size, 1, sizeof(off64_t), archive);
		if(current_endianness == ENDIANNESS_LITTLE)
			flip(&size, sizeof(off64_t));
		_fseeki64(archive, size, SEEK_CUR);
		lengthOfCurrentFileName = 0;
		goto follow_file;
	}

		

	remove(helpFileName);
	return 0;
}
