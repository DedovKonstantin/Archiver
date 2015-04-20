#include "HEADER/HeaderArchiver.h"

/*********************************************************************************
*add - функция добавляет директорию с именем fileNameOfInstered со всеми файлами.
*Возвращает	0 при успешном добавлении
*			ERROR_MEMORY_DOES_NOT_ALLOCATE при нехватке динамической памяти
*			ERROR_ARCHIVE_IS_NOT_WRITE, если архив закрыт на запись
*********************************************************************************/

int add_directory(const char *directoryName, const char *archiveName, const char *helpFileName){
	
	DWORD currentLengthOfPathName;
	wchar_t lpThisCatalogName[BLOCK_NAME];
	DWORD nBufferLength = BLOCK_NAME;
	wchar_t lpBuffer[BLOCK_NAME];
	wchar_t lpDirectoryName[BLOCK_NAME];
	char buffer[BLOCK_NAME];
	char cFileName[BLOCK_NAME];
	LPWSTR lpFilePart;

	WIN32_FIND_DATAW FindFileData;
	HANDLE hf;
	struct __stat64 st;
	int directoryIsEmpty = 1;

	int return_error;
	char isZipped;

	
	
	
	charToLPWSTR(THIS_DIRECTORY, lpThisCatalogName);
	GetFullPathNameW
				(
				lpThisCatalogName,
				nBufferLength,
				lpBuffer,
				&lpFilePart
				);
	currentLengthOfPathName = strlenLPWSTR(lpBuffer);

	charToLPWSTR(directoryName, lpDirectoryName);

	wcscat(lpBuffer, (LPWSTR)SEPARATOR_PATH_NAME_STR);
	wcscat(lpBuffer, lpDirectoryName);
	wcscat(lpBuffer, (LPWSTR)SEPARATOR_PATH_NAME_STR);
	wcscat(lpBuffer, L"*");
	hf=FindFirstFileW(lpBuffer, &FindFileData);
	if (hf!=INVALID_HANDLE_VALUE)
	{
		do
		{
			LPWSTRtoChar(lpBuffer, buffer);
			LPWSTRtoChar(FindFileData.cFileName, cFileName);
			strcpy(buffer, directoryName);
			strcat(buffer, SEPARATOR_PATH_NAME_STR);
			strcat(buffer, cFileName);
			__stat64(buffer, &st);
			if(!strcmp(cFileName, THIS_DIRECTORY) || !strcmp(cFileName, PARENT_DIRECTORY))
				continue;
			directoryIsEmpty = 0;
			


			
			if(st.st_mode & S_IFDIR)
			{
				if(add_directory(buffer, archiveName, helpFileName) == ERROR_MEMORY_DOES_NOT_ALLOCATE)
					return ERROR_MEMORY_DOES_NOT_ALLOCATE;
			}
			else
			{
				if(return_error = Huffman(OPERATION_ZIP, buffer, helpFileName, &isZipped)){
					if(return_error == ERROR_MEMORY_DOES_NOT_ALLOCATE){
							remove(helpFileName);
							return ERROR_MEMORY_DOES_NOT_ALLOCATE;
					}
					if(return_error == ERROR_FILE_NOT_FOUND_IN_DIRECTORY){
						printf("File \"%s\" not found in directory.\n", buffer);
						continue;
					}
					

				}
				if(isZipped){
					if(return_error = add(helpFileName, buffer, archiveName, isZipped)){
					
						if(return_error == SKIP_FILE_ADD){
							printf("File \"%s\" skipped.\n", buffer);
							continue;
						}
						if(return_error == REPLACE_FILE_ADD){
							printf("File \"%s\" replaced.\n", buffer);
							continue;
						}
						if(return_error == ERROR_ARCHIVE_IS_NOT_WRITE){
							remove(helpFileName);
							return ERROR_ARCHIVE_IS_NOT_WRITE;
						}
					}
				}else{
					if(return_error = add(buffer, buffer, archiveName, isZipped)){
						
						if(return_error == SKIP_FILE_ADD){
							printf("File \"%s\" skipped.\n", buffer);
							continue;
						}
						if(return_error == REPLACE_FILE_ADD){
							printf("File \"%s\" replaced.\n", buffer);
							continue;
						}
						if(return_error == ERROR_ARCHIVE_IS_NOT_WRITE){
							return ERROR_ARCHIVE_IS_NOT_WRITE;
						}
					}
				}
				remove(helpFileName);
				printf("File \"%s\" added.\n", buffer);
			}
			
		}while (FindNextFileW(hf,&FindFileData)!=0);
		
		if(directoryIsEmpty){

			buffer[strlen(buffer)-2] = EMPTY_DIRECTORY;
			buffer[strlen(buffer)-1] = '\0';
 			if(return_error = add(helpFileName, buffer, archiveName, 0))
			{
				buffer[strlen(buffer)-1] = '\0';
				if(return_error == SKIP_FILE_ADD){
					printf("Empty directory \"%s\" skipped.\n", buffer);
				}
				if(return_error == REPLACE_FILE_ADD){
					printf("Empty directory \"%s\" replaced.\n", buffer);
				}
				if(return_error == ERROR_ARCHIVE_IS_NOT_WRITE){
					return ERROR_ARCHIVE_IS_NOT_WRITE;
				}
			}

			if(!return_error)
			{
				buffer[strlen(buffer)-1] = '\0';
				printf("Empty directory \"%s\" added.\n", buffer);
			}
		}
		FindClose(hf);
	}
	remove(helpFileName);
	return 0;
}
