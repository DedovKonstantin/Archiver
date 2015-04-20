#include "HEADER/HeaderArchiver.h"

/*********************************************************************************************
*extract - функция извлечения файла из архива (без удаления и разархивации)
*
*fileName - имя файла в архиве,
*archive_name - имя архива.
*newFileName - сюда будет установлено окончательное имя разархивированного файла.
*iszip - сюда будет установлено состояние файла(сжат/не сжат).
*
*Возвращает ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE, если файла с именем fileName в архиве нет.
*			SKIP_FILE_EXTRACT, если файл пропускается.
*			REPLACE_FILE_EXTRACT, если файл заменяется.
*			ERROR_INCORRECT_CHECKSUM, если извлекаемый файл повреждён.
*			REPLACE_FILE_EXTRACT, если извлекаемый файл был заменён существующим.
*			SKIP_FILE_EXTRACT, если файл пропускается,
*			ERROR_DIRECTORY_NAME_TOO_LONG, если имя директории слишком длинное для создания.
*********************************************************************************************/
#define SKIP_ONE_FILE 1
#define SKIP_ALL_FILES 2
#define REPLACE_ONE_FILE 3
#define REPLACE_ALL_FILES 4
#define CHOOSE_NEW_NAME__ONE_FILE 5
#define CHOOSE_NEW_NAME__ALL_FILES 6

#define SKIP_ONE_FILE_READ_ONLY 1
#define SKIP_ALL_FILES_READ_ONLY 2
#define CHOOSE_NEW_NAME__ONE_FILE_READ_ONLY 3
#define CHOOSE_NEW_NAME__ALL_FILES_READ_ONLY 4

#define SKIP_ONE_FILE_NAME_OF_FILE_TOO_LONG 1
#define SKIP_ALL_FILES_NAME_OF_FILE_TOO_LONG 2
#define CHOOSE_NEW_NAME__ONE_FILE_NAME_OF_FILE_TOO_LONG 3
#define CHOOSE_NEW_NAME__ALL_FILES_NAME_OF_FILE_TOO_LONG 4

int extract(const char *fileName, const char *archive_name, char *iszip, char *newFileName){
	static int replace_all = 0, skip_all = 0, chooseNewName_all = 0;
	static int skip_all_read_only = 0, chooseNewName_all_read_only = 0;
	static int skip_all_too_long = 0, choose_new_name_all_too_long = 0;

	char currentSymbols[BLOCK];
	FILE *extracted, *archive;
	char checksum[CHECKSUM_LENGTH + 1];
	char currentFileName[BLOCK_NAME];
	off64_t size, size_archive, sizeRead, currentSize = 0;
	time_t ctime;
	char sym;
	int lengthOfCurrentFileName = 0;
	char isZipped;
	DWORD attributes;
	wchar_t fileNameLPWSTR[BLOCK_NAME];
	int file_is_replace = 0;
	int mode;
	unsigned __int64 CRC_v, CRC_current_v = 0;
	off64_t sizeOfMeta;
	struct __stat64 info_archive;
	off64_t currentSizeOfArchive = 0;

	DWORD currentLengthOfPathName;
	wchar_t lpThisCatalogName[BLOCK_NAME];
	DWORD nBufferLength = BLOCK_NAME;
	wchar_t lpBuffer[BLOCK_NAME];
	char buffer[BLOCK_NAME];
	LPWSTR lpFilePart;

	extern current_endianness;



	archive = fopen(archive_name, "rb");
	__stat64(archive_name, &info_archive);
	fread(checksum, 1, CHECKSUM_LENGTH+1, archive);
	currentSizeOfArchive += CHECKSUM_LENGTH+1;
	strcpy(newFileName, fileName);

	charToLPWSTR(THIS_DIRECTORY, lpThisCatalogName);
	GetFullPathNameW
				(
				lpThisCatalogName,
				nBufferLength,
				lpBuffer,
				&lpFilePart
				);
	currentLengthOfPathName = strlenLPWSTR(lpBuffer);


follow_file:
	while((++currentSizeOfArchive <= info_archive.st_size)&&(sym = fgetc(archive)) && (sym != END_OF_ARCHIVE) /*&&(sym != SEPARATOR_PATH_NAME_CHR)*/ && (sym != EMPTY_DIRECTORY))
	{
		currentFileName[lengthOfCurrentFileName++] = sym;
	}
	currentFileName[lengthOfCurrentFileName] = '\0';
	if(currentSizeOfArchive > info_archive.st_size){
		fclose(archive);
		return ERROR_INCORRECT_CHECKSUM;
	}

	if(sym == END_OF_ARCHIVE){
		fclose(archive);
		return ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE;
	}

	if(!strcmp(currentFileName, fileName) || sym == EMPTY_DIRECTORY)
	{
		_fseeki64(archive, -lengthOfCurrentFileName-1, SEEK_CUR);
		currentSizeOfArchive -= lengthOfCurrentFileName+1;
		lengthOfCurrentFileName = 0;
		while((++currentSizeOfArchive <= info_archive.st_size)&&(sym = fgetc(archive)) && (sym != END_OF_ARCHIVE) &&(sym != SEPARATOR_PATH_NAME_CHR) && (sym != EMPTY_DIRECTORY))
		{
continue_read_fileName_gt:
			currentFileName[lengthOfCurrentFileName++] = sym;
		}
		if(currentSizeOfArchive > info_archive.st_size){
			fclose(archive);
			return ERROR_INCORRECT_CHECKSUM;
		}
		currentFileName[lengthOfCurrentFileName] = '\0';

		if(sym == SEPARATOR_PATH_NAME_CHR || sym == EMPTY_DIRECTORY)
		{
			if(sym == EMPTY_DIRECTORY){
				currentFileName[lengthOfCurrentFileName++] = sym;
				currentFileName[lengthOfCurrentFileName++] = '\0';
				
				if(!strcmp(currentFileName, fileName)){
					
					*iszip = 0;
					fclose(archive);
					return 0;
				}
				else
				{	
					++currentSizeOfArchive;
					sym = fgetc(archive);


					lengthOfCurrentFileName = 0;
					goto follow_file;
				}

			}
			charToLPWSTR(THIS_DIRECTORY, lpThisCatalogName);
			GetFullPathNameW
			(
				lpThisCatalogName,
				nBufferLength,
				lpBuffer,
				&lpFilePart
			);

			LPWSTRtoChar(lpBuffer, buffer);
			strcat(buffer, SEPARATOR_PATH_NAME_STR);
			strcat(buffer, currentFileName);

			if((currentLengthOfPathName + strlen(buffer)) >= BLOCK_NAME)
			{
				printf
					(	
					"Error: can't create new directory (\"%s\") because "
					"length of name of file in this directory must be not more than %d\n",
					buffer,
					BLOCK_NAME-1 - currentLengthOfPathName
					);

				fclose(archive);
				return ERROR_DIRECTORY_NAME_TOO_LONG;
			}


			mkdir(buffer);
			
			
			goto continue_read_fileName_gt;
		}
	}



	
	if(strcmp(currentFileName, fileName)){
		if((currentSizeOfArchive += sizeof(off64_t)) > info_archive.st_size){
			fclose(archive);
			return ERROR_INCORRECT_CHECKSUM;
		}
		fread(&size, 1, sizeof(off64_t), archive);
		if(current_endianness == ENDIANNESS_LITTLE)
			flip(&size, sizeof(off64_t));
		if((currentSizeOfArchive += size) > info_archive.st_size || size < 0){
			fclose(archive);
			return ERROR_INCORRECT_CHECKSUM;
		}
		_fseeki64(archive, size, SEEK_CUR);
		lengthOfCurrentFileName = 0;
		goto follow_file;
	}

	if((currentLengthOfPathName + strlen(currentFileName)) >= BLOCK_NAME){
		if(skip_all_too_long)
			goto skip_gt;
		if(choose_new_name_all_too_long)
			goto choose_new_name_gt;
incorrect_comand_too_long:
		printf
			(
			"Length of name of file (\"%s\") in this directory must be not more than %d\n"
			"Print here:\n"
			"\t\t\t%d - if you want skip this file;\n"
			"\t\t\t%d - if you want skip all the same files;\n"
			"\t\t\t%d - if you want choose new name for this file;\n"
			"\t\t\t%d - if you want choose new name for any the same file.\n",
			currentFileName,
			BLOCK_NAME-1 - currentLengthOfPathName,
			SKIP_ONE_FILE_NAME_OF_FILE_TOO_LONG,
			SKIP_ALL_FILES_NAME_OF_FILE_TOO_LONG,
			CHOOSE_NEW_NAME__ONE_FILE_NAME_OF_FILE_TOO_LONG,
			CHOOSE_NEW_NAME__ALL_FILES_NAME_OF_FILE_TOO_LONG
			);
		scanf("%d", &mode);
		skip_input();
		switch(mode){
			case SKIP_ONE_FILE_NAME_OF_FILE_TOO_LONG: 
					goto skip_gt;
			case SKIP_ALL_FILES_NAME_OF_FILE_TOO_LONG: 
					skip_all_too_long = 1;
					goto skip_gt;
			case CHOOSE_NEW_NAME__ONE_FILE_NAME_OF_FILE_TOO_LONG: 
					printf
						(
						"Length of name of file in this directory must be not more than %d\n"
						"Choose correct name: ",
						BLOCK_NAME-1 - currentLengthOfPathName
						);
					goto new_name_incorrect;
			case CHOOSE_NEW_NAME__ALL_FILES_NAME_OF_FILE_TOO_LONG:
					choose_new_name_all_too_long = 1;
					printf
						(
						"Length of name of file in this directory must be not more than %d\n"
						"Choose correct name: ",
						BLOCK_NAME-1 - currentLengthOfPathName
						);
					goto new_name_incorrect;
			default:
				printf("Incorrect comand.\n");
				skip_input();
				goto incorrect_comand_too_long;
		}

		goto new_name_incorrect;
	}



continue_check:
	if(access(newFileName, 0) != -1){

		if(replace_all){
replace_gt:
			file_is_replace = 1;
			goto new_name;
		}
		if(skip_all){
skip_gt:
			return SKIP_FILE_EXTRACT;
		}
		if(chooseNewName_all){
choose_new_name_gt:
			printf("File with name \"%s\" is exist in directory. Choose new name:\n", newFileName);
new_name_incorrect:
			printf("%s", newFileName);
			editText(&newFileName);
			if((currentLengthOfPathName + strlen(newFileName)) >= BLOCK_NAME){
				printf
					(
					"Length of name of file in this directory must be not more than %d\n"
					"Choose correct name:\n",
					BLOCK_NAME-1 - currentLengthOfPathName
					);
				goto new_name_incorrect;
			}

			if(!checkFileName_Windows(newFileName)){
			printf
				(
				"Name of file can't contains one of follow symbols: /, :, *, ?, \", <, >, |\n"
				"Choose correct name:\n"
				);
			goto new_name_incorrect;
			}

			goto continue_check;
		}
incorrect_comand:
		printf
			(
			"File with name \"%s\" is exist in directory.\n"
			"Print here:\n"
			"\t\t\t%d - if you want skip this file;\n"
			"\t\t\t%d - if you want skip all files;\n"
			"\t\t\t%d - if you want replace this file;\n"
			"\t\t\t%d - if you want replace all files;\n"
			"\t\t\t%d - if you want choose new name for this file;\n"
			"\t\t\t%d - if you want choose new name for any file.\n",
			newFileName,
			SKIP_ONE_FILE,
			SKIP_ALL_FILES,
			REPLACE_ONE_FILE,
			REPLACE_ALL_FILES,
			CHOOSE_NEW_NAME__ONE_FILE,
			CHOOSE_NEW_NAME__ALL_FILES
		);
		scanf("%d", &mode);
		skip_input();
		switch(mode){
			case SKIP_ONE_FILE: 
					goto skip_gt;
			case SKIP_ALL_FILES: 
					skip_all = 1;
					goto skip_gt;
			case REPLACE_ONE_FILE: 
					goto replace_gt;
			case REPLACE_ALL_FILES:
					replace_all = 1;
					goto replace_gt;
			case CHOOSE_NEW_NAME__ONE_FILE:
					goto choose_new_name_gt;
			case CHOOSE_NEW_NAME__ALL_FILES:
					chooseNewName_all = 1;
					goto choose_new_name_gt;

			default:
				printf("Incorrect comand.\n");
				skip_input();
				goto incorrect_comand;
		}
		goto continue_check;
	}


new_name:
	if(((extracted = fopen(newFileName, "wb")) == NULL)||(!access(newFileName, 0) && access(newFileName, 2) == -1)){

		if(skip_all_read_only){
skip_read_only_gt:
			return SKIP_FILE_EXTRACT;
		}
		
		
		
		if(chooseNewName_all_read_only){
choose_new_name_read_only_gt:
			printf("File with name \"%s\" is not write. Choose new name:\n", newFileName);
new_name_incorrect_read_only:
			printf("%s", newFileName);
			editText(&newFileName);
			if((currentLengthOfPathName + strlen(newFileName)) >= BLOCK_NAME){
				printf
					(
					"Length of name of file in this directory must be not more than %d\n"
					"Choose correct name:\n",
					BLOCK_NAME-1 - currentLengthOfPathName
					);
				goto new_name_incorrect_read_only;
			}
			if(!checkFileName_Windows(newFileName)){
			printf
				(
				"Name of file can't contains one of follow symbols: /, :, *, ?, \", <, >, |\n"
				"Choose correct name:\n"
				);
			goto new_name_incorrect_read_only;
			}
			file_is_replace = 0;
			goto continue_check;
		}

incorrect_comand_read_only:
		printf
			(
			"File with name \"%s\" is not write.\n"
			"Print here:\n"
			"\t\t\t%d - if you want skip this file;\n"
			"\t\t\t%d - if you want skip all files;\n"
			"\t\t\t%d - if you want choose new name for this file;\n"
			"\t\t\t%d - if you want choose new name for any file.\n", 
			newFileName,
			SKIP_ONE_FILE_READ_ONLY,
			SKIP_ALL_FILES_READ_ONLY,
			CHOOSE_NEW_NAME__ONE_FILE_READ_ONLY,
			CHOOSE_NEW_NAME__ALL_FILES_READ_ONLY
			);
		scanf("%d", &mode);
		switch(mode){
			case SKIP_ONE_FILE_READ_ONLY: 
					goto skip_read_only_gt;
			case SKIP_ALL_FILES_READ_ONLY: 
					skip_all_read_only = 1;
					goto skip_read_only_gt;
			case CHOOSE_NEW_NAME__ONE_FILE_READ_ONLY:
					goto choose_new_name_read_only_gt;
			case CHOOSE_NEW_NAME__ALL_FILES_READ_ONLY:
					chooseNewName_all_read_only = 1;
					goto choose_new_name_read_only_gt;
			default:
				printf("Incorrect comand.\n");
				skip_input();
				goto incorrect_comand_read_only;
		}
		goto continue_check;
	}


/******************************МЕТАДАННЫЕ**********************************/

	if((currentSizeOfArchive += sizeof(off64_t)) > info_archive.st_size){
		fclose(archive);
		fclose(extracted);
		remove(newFileName);
		return ERROR_INCORRECT_CHECKSUM;
	}
	freadWithCRC(&size,	sizeof(off64_t),	archive, &CRC_current_v);
	if(current_endianness == ENDIANNESS_LITTLE){
		flip(&size,	sizeof(off64_t));
	}
	if((currentSizeOfArchive += size) > info_archive.st_size || size < 0){
		fclose(archive);
		fclose(extracted);
		remove(newFileName);
		return ERROR_INCORRECT_CHECKSUM;
	}

	freadWithCRC(&isZipped,		sizeof(char),	archive, &CRC_current_v);
	freadWithCRC(&size_archive,	sizeof(off64_t),	archive, &CRC_current_v);
	freadWithCRC(&ctime,		sizeof(time_t), archive, &CRC_current_v);
	freadWithCRC(&attributes,	sizeof(DWORD),	archive, &CRC_current_v);
	if(current_endianness == ENDIANNESS_LITTLE){
			flip(&size_archive,	sizeof(off64_t));
			flip(&ctime,		sizeof(time_t));
			flip(&attributes,	sizeof(DWORD));
	}
	if(isZipped)
		*iszip = 1;
	else
		*iszip = 0;

	sizeOfMeta = sizeof(char)//флаг сжатия
				+sizeof(off64_t)//размер исходного файла
				+sizeof(time_t)//дата создания файла
				+sizeof(DWORD)//attributes of file
				+sizeof(unsigned __int64);//CRC
	
	
	
/**************************************************************************/

	while(currentSize + BLOCK <= size - sizeOfMeta)
	{
		freadWithCRC(currentSymbols, BLOCK, archive, &CRC_current_v);
		fwrite(currentSymbols, sizeof (char), BLOCK, extracted);
		currentSize += BLOCK;
	}

	freadWithCRC(currentSymbols, sizeRead = size - sizeOfMeta - currentSize, archive, &CRC_current_v);
	fwrite(currentSymbols, sizeof (char),sizeRead, extracted);
	fread(&CRC_v, 1, sizeof(unsigned __int64), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&CRC_v, sizeof(unsigned __int64));
	
	if(CRC_current_v != CRC_v){
		fclose(archive);
		fclose(extracted);
		remove(newFileName);
		return ERROR_INCORRECT_CHECKSUM;
	}

	charToLPWSTR(newFileName, fileNameLPWSTR);
	SetFileAttributesW(fileNameLPWSTR, attributes); 
	
	fclose(archive);
	fclose(extracted);
	if(file_is_replace)
		return REPLACE_FILE_EXTRACT;
	return 0;

}
