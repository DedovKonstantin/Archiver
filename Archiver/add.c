#include "HEADER/HeaderArchiver.h"

/*********************************************************************************
*add - функция добавляет файл с именем fileNameOfInstered
*	в архив с именем archive_name и придаёт этому файлу
*	в архиве имя newFileName.
*iszip - указывает, является ли файл сжатым.
*	Если файл с таким именем уже существует в архиве, то функция
*	выдаст запрос на дальнейшие действия.

*Возвращает	0 при успешном добавлении
*			REPLACE_FILE_ADD, если файл в архиве был заменён
*			SKIP_FILE_ADD, если файл в архиве был пропущен
*			ERROR_ARCHIVE_IS_NOT_WRITE, если архив закрыт на запись
*********************************************************************************/

#define SKIP_ONE_FILE 1
#define SKIP_ALL_FILES 2
#define REPLACE_ONE_FILE 3
#define REPLACE_ALL_FILES 4
#define CHOOSE_NEW_NAME__ONE_FILE 5
#define CHOOSE_NEW_NAME__ALL_FILES 6

int add(const char *fileNameOfInstered, const char *newFileName, const char *archive_name, const char iszip)
{
	static int replace_all = 0, skip_all = 0, chooseNewName_all = 0;

	char currentSymbols[BLOCK];
	char checksum[CHECKSUM_LENGTH + 1];
	FILE *archive, *inserted;
	off64_t size, sizeRead, currentSize = 0;
	char sym;
	int lengthOfCurrentFileName = 0;
	char currentFileName[BLOCK_NAME];
	struct __stat64 state, stateOld;
	char newFileNameHelp[BLOCK_NAME];
	int mode;
	DWORD attributes;
	wchar_t newFileNameLPWSTR[BLOCK_NAME];
	unsigned __int64 CRC_v = 0;
	unsigned countOfFiles = 0;
	off64_t sizeOfMeta;
	extern current_endianness;

	if((archive = fopen(archive_name, "rb+")) == NULL){
		archive = fopen(archive_name, "wb");
		fwrite(CHECKSUM, 1, CHECKSUM_LENGTH, archive);
		fputc('\0', archive);
		fputc(END_OF_ARCHIVE, archive);
		inserted = fopen(fileNameOfInstered, "rb");
		strcpy(newFileNameHelp, newFileName);
		goto add_gt;
	}else{
		fread(checksum, 1, CHECKSUM_LENGTH + 1, archive);
	}

	inserted = fopen(fileNameOfInstered, "rb");
	strcpy(newFileNameHelp, newFileName);
follow_file:
new_name:
	while((sym = fgetc(archive)) && sym != END_OF_ARCHIVE){
		currentFileName[lengthOfCurrentFileName++] = sym;
	}
	if(sym == END_OF_ARCHIVE){
		goto add_gt;
	}
	countOfFiles++;
	currentFileName[lengthOfCurrentFileName] = '\0'; 
	if(strcmp(currentFileName, newFileNameHelp)){
		if(currentFileName[strlen(currentFileName)-1] == EMPTY_DIRECTORY)
			goto follow_file;
		fread(&size, 1, sizeof(off64_t), archive);
		if(current_endianness == ENDIANNESS_LITTLE)
			flip(&size, sizeof(off64_t));
		_fseeki64(archive, size, SEEK_CUR);
		lengthOfCurrentFileName = 0;
		goto follow_file;
		
	}else
	{
		if(replace_all){
replace_gt:
			fclose(archive);
			if(inserted)
				fclose(inserted);
			removeElement(newFileName, archive_name);
			add(fileNameOfInstered, newFileName, archive_name, iszip);
			return REPLACE_FILE_ADD;
		}
		if(skip_all){
skip_gt:
			fclose(archive);
			if(inserted)
				fclose(inserted);
			return SKIP_FILE_ADD;
		}
		if(chooseNewName_all){
choose_new_name_gt:
			printf("File with name \"%s\" is exist. Choose new name:\n", newFileNameHelp);
new_name_incorrect:
			printf("%s", newFileNameHelp);
			editText_static(&newFileNameHelp); 

			if(!checkFileName_Windows(newFileNameHelp)){
			printf
				(
				"Name of file can't contains one of follow symbols: /, :, *, ?, \", <, >, |\n"
				"Choose correct name: "
				);
			goto new_name_incorrect;
			}
			_fseeki64(archive, CHECKSUM_LENGTH + 1, SEEK_SET);
			lengthOfCurrentFileName = 0;
		
			goto new_name;
		}
incorrect_comand:
		printf
			(
			"File with name \"%s\" is exist.\n"
			"Print here:\n"
			"\t\t\t%d - if you want skip this file;\n"
			"\t\t\t%d - if you want skip all files;\n"
			"\t\t\t%d - if you want replace this file;\n"
			"\t\t\t%d - if you want replace all files;\n"
			"\t\t\t%d - if you want choose new name for this file;\n"
			"\t\t\t%d - if you want choose new name for any file.\n",
			newFileNameHelp,
			SKIP_ONE_FILE,
			SKIP_ALL_FILES,
			REPLACE_ONE_FILE,
			REPLACE_ALL_FILES,
			CHOOSE_NEW_NAME__ONE_FILE,
			CHOOSE_NEW_NAME__ALL_FILES
		);
		scanf("%d", &mode);
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
					
	}

add_gt:
	fclose(archive);
	if((archive = fopen(archive_name, "rb+")) == NULL)
		return ERROR_ARCHIVE_IS_NOT_WRITE;
	_fseeki64(archive, -1, SEEK_END);//к END_OF_ARCHIVE
	__stat64(fileNameOfInstered, &state);
	__stat64(newFileName, &stateOld);
	
/*********************************МЕТАДАННЫЕ************************************/
	fprintf(archive, "%s", newFileNameHelp);
	fputc('\0', archive);
	if(newFileNameHelp[strlen(newFileNameHelp) - 1] == EMPTY_DIRECTORY){
		fputc(END_OF_ARCHIVE, archive);
		if(inserted)
			fclose(inserted);
		fclose(archive);
		return 0;
	}
			



	charToLPWSTR(newFileName, newFileNameLPWSTR);
	attributes = GetFileAttributesW(newFileNameLPWSTR); 
	sizeOfMeta = sizeof(char)//флаг сжатия
				+sizeof(off64_t)//размер исходного файла
				+sizeof(time_t)//дата создания файла
				+sizeof(DWORD)//attributes of file
				+sizeof(unsigned __int64);//CRC
	
	state.st_size += sizeOfMeta;

	if(current_endianness == ENDIANNESS_LITTLE){
		flip(&state.st_size,	sizeof(off64_t));
		flip(&stateOld.st_size, sizeof(off64_t));
		flip(&state.st_ctime,	sizeof(time_t));
		flip(&attributes,		sizeof(DWORD));
	}
	fwriteWithCRC(&state.st_size,		sizeof(off64_t), archive, &CRC_v);//размер архива
	fwriteWithCRC(&iszip,				sizeof(char), archive, &CRC_v);//флаг сжатия
	fwriteWithCRC(&stateOld.st_size,	sizeof(off64_t), archive, &CRC_v);//размер исходного файла
	fwriteWithCRC(&state.st_ctime,		sizeof(time_t), archive, &CRC_v);//дата создания файла
	fwriteWithCRC(&attributes,			sizeof(DWORD), archive, &CRC_v);//attributes of file

	if(current_endianness == ENDIANNESS_LITTLE){
		flip(&state.st_size,	sizeof(off64_t));
		flip(&stateOld.st_size, sizeof(off64_t));
		flip(&state.st_ctime,	sizeof(time_t));
		flip(&attributes,		sizeof(DWORD));
	}
/*******************************************************************************/

	while(currentSize + BLOCK <= state.st_size - sizeOfMeta)
	{
		fread(currentSymbols, sizeof (char), BLOCK, inserted);
		fwriteWithCRC(currentSymbols, BLOCK, archive, &CRC_v);
		currentSize += BLOCK;
	}
	fread(currentSymbols, sizeof (char), sizeRead = state.st_size - sizeOfMeta - currentSize, inserted);
	fwriteWithCRC(currentSymbols, sizeRead, archive, &CRC_v);

	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&CRC_v, sizeof(unsigned __int64));
	fwrite(&CRC_v, 1, sizeof(unsigned __int64), archive);//CRC
	fputc(END_OF_ARCHIVE, archive);

	fclose(archive);
	if(inserted)
		fclose(inserted);
	return 0;
}
