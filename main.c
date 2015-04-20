#include "Archiver/HEADER/HeaderArchiver.h"

#define ENGLISH_LANGUAGE 'e'
#define RUSSIAN_LANGUAGE 'r'

int current_endianness;
int main(int argv, char *args[])
{
	char operation;
	char *archiveName;
	char isZipped;
	int i;
	char helpFileName[BLOCK_NAME];
	struct __stat64 st; 
	int return_error;
	char lang;
	wchar_t helpFileNameLPWSTR[BLOCK_NAME];
	char newFileName[BLOCK_NAME];
	FILE *help;

#ifdef FOR_RUSSIAN
	setlocale(LC_ALL, "Russian");
#endif

	if(argv == 2 && !strcmp(args[1], "/help")){
		printf("Choose language ('%c' - Russian, '%c' - English): ", RUSSIAN_LANGUAGE, ENGLISH_LANGUAGE);
		lang = getc(stdin);
		lang = tolower(lang);
		if(lang == ENGLISH_LANGUAGE)
			printf
				(
				"This program can compress your file(s), add in archive and unzip their in future\n"
				"Format of parameters: symbol of operation, name of archive, name of file, name of file, .../n"
				"Operation 'a' - add file(s) to archive;\n"
				"          'x' - extract file(s) from archive;\n"
				"          'i' - get information about archive and its file(s),\n"
				"                you can print fourth parametr 'd' or 'c': method to show (dense or comfortably);\n"
				"          'l' - get information about files, which printed on parameters.\n"
				"          'd' - delete file(s) from archive (not unzip!);\n"
				"          't' - check archive;\n"
				"          'u' - unzip archive - extract all files from archive;\n"
				"          'f' - delete all files from archive (not unzip!);\n"
				"          'p' - peek (unzip and remove) file(s) from archive.\n"
				);
		else{
			setlocale(LC_ALL, "Russian");
			printf
				(
				"Данная программа позволяет упаковать/распаковать ваши файлы со сжатием.\n"
				"Формат параметров: операция, имя архива, имя файла, имя файла, .../n"
				"Возможные операции: 'a' - добавить файл(ы) в архив;\n"
				"                    'x' - извлечь файл(ы) из архива;\n"
				"                    'i' - получить информацию об архиве и его содержимом,\n"
				"                          Вы можете подать четвёртый параметр - способ отображения информации - \n"
				"                          c - удобный, d - плотный (по умолчанию);\n"
				"                    'l' - получить информацию о конкретных файлах, поданных в параметрах.\n"
				"                    'd' - удалить файл(ы) из архива без распаковки;\n"
				"                    't' - проверить архив на целостность;\n"
				"                    'u' - извлечь все файлы из архива;\n"
				"                    'f' - удалить все файлы из архива;\n"
				"                    'p' - извлечь файл(ы) с последующем удалением из архива.\n"
				);
#ifndef FOR_RUSSIAN
			setlocale(LC_ALL, "English");
#endif
		}
		return 0;
	}

	if(argv <= 2){
		printf("Not enouth arguments. Print \"/help\" to get help.\n");
		return ERROR_NOT_ENOUTH_ARGUMENTS;
	}
	current_endianness = endianness();
	operation = args[1][0];
	archiveName = args[2];
	switch(operation){
		case INSERT:
		case REMOVE:
		case FREE:
		case PEEK: if(archiveNotWrite(archiveName)){
						printf(ERROR_FILE_IS_NOT_WRITE_MESSAGE, archiveName);
						return ERROR_FILE_IS_NOT_WRITE;
				   }
				   if(!checkForWrite(archiveName, operation))
					   return ERROR_INCORRECT_CHECKSUM;
				   break;
		case CHECK:
			break;
		default: if(!checkForRead(archiveName, operation)){
					return ERROR_ARCHIVE_NOT_FOUND;
				 }
	}


	if(operation == CHECK){
		if(__stat64(archiveName, &st) == -1){
			printf("Archive \"%s\" not found", archiveName);
			return 0;
		}
		if(checkForWrite(archiveName, operation))
			printf("Archive \"%s\" is not corrupted.\n", archiveName);
		return 0;
	}

	


	if(operation == INSERT)
	{
		tmpnam(helpFileName);
		shiftLeftStringOne(helpFileName);
		if((help = fopen(helpFileName, "wb"))==NULL){
			printf("Error: this directory is not for write.\n");
			return ERROR_DIRECTORY_IS_NOT_FOR_WRITE;
		}
		fclose(help);
		for(i = 3; i < argv; i++){
			__stat64(args[i], &st);
			if(st.st_mode & S_IFDIR)
			{
				if(return_error = add_directory(args[i], archiveName, helpFileName)){
					if(return_error == ERROR_MEMORY_DOES_NOT_ALLOCATE){
						printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
						remove(helpFileName);
						return ERROR_MEMORY_DOES_NOT_ALLOCATE;
					}
					if(return_error == ERROR_ARCHIVE_IS_NOT_WRITE){
						printf(ERROR_FILE_IS_NOT_WRITE_MESSAGE, archiveName);
						remove(helpFileName);
						return ERROR_ARCHIVE_IS_NOT_WRITE;
					}
				}
				continue;
			}
			if(return_error = Huffman(OPERATION_ZIP, args[i], helpFileName, &isZipped)){
				if(return_error == ERROR_MEMORY_DOES_NOT_ALLOCATE){
						printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
						remove(helpFileName);
						return ERROR_MEMORY_DOES_NOT_ALLOCATE;
				}
				if(return_error == ERROR_FILE_NOT_FOUND_IN_DIRECTORY){
					printf("File \"%s\" not found in directory.\n", args[i]);
					continue;
				}
				

			}
			if(isZipped){
				if(return_error = add(helpFileName, args[i], archiveName, isZipped)){
				
					if(return_error == SKIP_FILE_ADD){
						printf("File \"%s\" skipped.\n", args[i]);
						continue;
					}
					if(return_error == REPLACE_FILE_ADD){
						printf("File \"%s\" replaced.\n", args[i]);
						continue;
					}
					if(return_error == ERROR_ARCHIVE_IS_NOT_WRITE){
						printf(ERROR_FILE_IS_NOT_WRITE_MESSAGE, archiveName);
						remove(helpFileName);
						return ERROR_ARCHIVE_IS_NOT_WRITE;
					}
				}
			}else{
				if(return_error = add(args[i], args[i], archiveName, isZipped)){
					
					if(return_error == SKIP_FILE_ADD){
						printf("File \"%s\" skipped.\n", args[i]);
						continue;
					}
					if(return_error == REPLACE_FILE_ADD){
						printf("File \"%s\" replaced.\n", args[i]);
						continue;
					}
					if(return_error == ERROR_ARCHIVE_IS_NOT_WRITE){
						printf(ERROR_FILE_IS_NOT_WRITE_MESSAGE, archiveName);
						remove(helpFileName);
						return ERROR_ARCHIVE_IS_NOT_WRITE;
					}
				}
			}
			remove(helpFileName);
			printf("File \"%s\" added.\n", args[i]);
		}
		remove(helpFileName);
	}
	else if(operation == INFORMATION_ALL){
		if(argv >= 4) {
			if(getInfo(archiveName, args[3][0], 1, ""))
				return ERROR_INCORRECT_CHECKSUM;
		}
		else{
			if(getInfo(archiveName, DENSE_INFO, 1, ""))
				return ERROR_INCORRECT_CHECKSUM;
		}
	}
	else if(operation == INFORMATION){
		for(i = 3; i < argv; i++)
			if(getInfo(archiveName, COMFORTABLY_INFO, 0, args[i]))
				return ERROR_INCORRECT_CHECKSUM;;
	}
	else if (operation == EXTRACT){
		tmpnam(helpFileName);
		shiftLeftStringOne(helpFileName);
		if((help = fopen(helpFileName, "wb"))==NULL){
			printf("Error: this directory is not for write.\n");
			return ERROR_DIRECTORY_IS_NOT_FOR_WRITE;
		}
		fclose(help);
		for(i = 3; i < argv; i++)
			if(return_error = extract(args[i], archiveName, &isZipped, newFileName)){
				if(return_error == SKIP_FILE_EXTRACT){
					printf("File \"%s\" skipped.\n", args[i]);
					continue;
				}
				if(return_error == ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE){
					printf("File \"%s\" not found in archive.\n", args[i]);
					continue;
				}
				if(return_error == ERROR_INCORRECT_CHECKSUM){
					printf("File \"%s\" corrupted.\n", args[i]);
					remove(helpFileName);
					return ERROR_INCORRECT_CHECKSUM;
				}
				if(return_error == ERROR_DIRECTORY_NAME_TOO_LONG){
					continue;
				}
				if(return_error == REPLACE_FILE_EXTRACT){
						goto unzip_gt;
				}
			}
			else{	
unzip_gt:
				if(isZipped){
					if(Huffman(OPERATION_UNZIP, helpFileName, newFileName, &isZipped)){
						if(ERROR_MEMORY_DOES_NOT_ALLOCATE){
							printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
							return ERROR_MEMORY_DOES_NOT_ALLOCATE;
						}
					}

					charToLPWSTR(newFileName, helpFileNameLPWSTR);
					SetFileAttributesW(helpFileNameLPWSTR ,STANDART_ATTRIBUTES);
					remove(newFileName);
					rename(helpFileName, newFileName);
				}
				if(return_error != REPLACE_FILE_EXTRACT)
					printf("File \"%s\" unzipped.\n", args[i]);
				else
					printf("File \"%s\" unzipped and replaced.\n", args[i]);

			}
			remove(helpFileName);

	}
	else if(operation == REMOVE){
		for(i = 3; i < argv; i++){
			if(return_error = removeElement(args[i], archiveName)){
				if(return_error == ERROR_MEMORY_DOES_NOT_ALLOCATE){
					printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
					remove(helpFileName);
					return ERROR_MEMORY_DOES_NOT_ALLOCATE;
				}
				if(return_error == ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE){
					printf("File \"%s\" is not found in archive.\n", args[i]);
					continue;
				}
				
			}
			printf("File \"%s\" is removed.\n", args[i]);
		}
	
	}else if(operation == EXTRACT_ALL){
		if(return_error = extractAll(archiveName)){
			if(return_error == ERROR_MEMORY_DOES_NOT_ALLOCATE){
				printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
				remove(helpFileName);
				return ERROR_MEMORY_DOES_NOT_ALLOCATE;
			}
		}
	}else if(operation == FREE){
		if((freeArchive(archiveName)) == ERROR_ARCHIVE_IS_NOT_WRITE){
			printf(ERROR_FILE_IS_NOT_WRITE_MESSAGE, archiveName);
			remove(helpFileName);
			return ERROR_ARCHIVE_IS_NOT_WRITE;
		};
		printf("Archive freed.\n");
	
	}else if (operation == PEEK){
		tmpnam(helpFileName);
		shiftLeftStringOne(helpFileName);
		if((help = fopen(helpFileName, "wb"))==NULL){
			printf("Error: this directory is not for write.\n");
			return ERROR_DIRECTORY_IS_NOT_FOR_WRITE;
		}
		fclose(help);
		for(i = 3; i < argv; i++){
			if(return_error = peekElement(args[i], archiveName, &isZipped, newFileName))
			{	
				if(return_error == ERROR_MEMORY_DOES_NOT_ALLOCATE){
					printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
					remove(helpFileName);
					return ERROR_MEMORY_DOES_NOT_ALLOCATE;
				}
				if(return_error == SKIP_FILE_EXTRACT){
					printf("File \"%s\" skipped.\n", args[i]);
					continue;
				}
				if(return_error == ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE){
					printf("File \"%s\" is not found in archive.\n", args[i]);
					continue;
				}
				if(return_error == ERROR_INCORRECT_CHECKSUM){
					printf("File \"%s\" corrupted.\n", args[i]);
					remove(helpFileName);
					return ERROR_INCORRECT_CHECKSUM;
				}
				if(return_error == REPLACE_FILE_EXTRACT){
					goto unzip_peek_gt;
				}

			}else
unzip_peek_gt:
				if(isZipped){
					if(Huffman(OPERATION_UNZIP, helpFileName, newFileName, &isZipped)){
							printf(ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE);
							remove(helpFileName);
							return ERROR_MEMORY_DOES_NOT_ALLOCATE;
					}
					charToLPWSTR(newFileName, helpFileNameLPWSTR);
					SetFileAttributesW(helpFileNameLPWSTR ,STANDART_ATTRIBUTES);
					remove(newFileName);
					rename(helpFileName, newFileName);
				}

				if(return_error != REPLACE_FILE_EXTRACT)
					printf("File \"%s\" unzipped and removed.\n", args[i]);
				else
					printf("File \"%s\" unzipped, replaced and removed.\n", args[i]);

		}
		remove(helpFileName);
	}else
		printf("Incorrect comand.\n");
	 

	return 0;

}
