#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <io.h>
#include <direct.h>
#include <conio.h>
#include "../../Huffman/HEADER/HeaderHuffman.h"

#ifndef HEADER_HUFFMAN
#include <stdio.h>
#include <string.h> 
#include <malloc.h>
#include <sys\stat.h>
#include <windows.h>
#define BLOCK 0x10000
#define BLOCK_NAME (MAX_PATH+1)
#define FOR_RUSSIAN
#define OPERATION_ZIP 'z'
#define OPERATION_UNZIP 'u'
#define ENDIANNESS_BIG 5
#define ENDIANNESS_LITTLE 6
#define ERROR_NOT_ENOUTH_ARGUMENTS (-1)
#define ERROR_MEMORY_DOES_NOT_ALLOCATE (-2)
#define ERROR_MEMORY_DOES_NOT_ALLOCATE_MESSAGE "Error: memory for new element(s) does not allocated.\n"
#define ERROR_FILE_IS_NOT_WRITE (-3)
#define ERROR_FILE_IS_NOT_WRITE_MESSAGE "Error: file \"%s\" is not for write.\n"
#define ERROR_FILE_NOT_FOUND_IN_DIRECTORY (-4)
#define ERROR_FILE_NOT_FOUND_ΙΝ_ARCHIVE (-5)
#define ERROR_ARCHIVE_NOT_FOUND (-6)
#define ERROR_INCORRECT_CHECKSUM (-7)
#define ERROR_ARCHIVE_IS_NOT_WRITE (-8)
#define ERROR_DIRECTORY_IS_NOT_FOR_WRITE (-9)
#define ERROR_DIRECTORY_NAME_TOO_LONG (-10)
#endif
 


//Сигнатура
#define CHECKSUM "IT IS MY ARCHIVE"
//Длина без нулевого символа; должна быть меньше, чем BLOCK_NAME-1.
#define CHECKSUM_LENGTH 16

#define DENSE_INFO 'd'
#define COMFORTABLY_INFO 'c' 

#define SKIP_FILE_EXTRACT 11
#define SKIP_FILE_ADD 11
#define REPLACE_FILE_ADD 12
#define REPLACE_FILE_EXTRACT 12

#define STANDART_ATTRIBUTES 32


#define END_OF_ARCHIVE			'|'
#define SEPARATOR_PATH_NAME_STR	        "\\"
#define SEPARATOR_PATH_NAME_CHR	        '\\'
#define EMPTY_DIRECTORY			'?'

#define THIS_DIRECTORY			"."
#define PARENT_DIRECTORY		".."



#define INSERT				'a'
#define EXTRACT				'x'
#define INFORMATION_ALL			'i'
#define INFORMATION			'l'

#define REMOVE				'd'
#define CHECK				't'

#define EXTRACT_ALL			'u'
#define FREE				'f'
#define PEEK				'p'


//Для редактирования текста в консоли
#define BUTTON_ENTER 13
#define BUTTON_ARROW_OR_DELETE 224
#define BUTTON_LEFT 75
#define BUTTON_RIGHT 77
#define BUTTON_DELETE 83
#define BUTTON_BACKSPACE 8
#define SEPARATOR 15

union meta{
	off64_t size_archive;
	char isZip;
	off64_t size_file;
	time_t ctime_v;
	DWORD attributes;
};

int add(const char *fileNameOfInstered, const char *newFileName, const char *archive_name, const char iszip);
int add_directory(const char *directoryName, const char *archive_name, const char *helpFileName);
int extract(const char *fileName, const char *archive_name, char *iszip, char *newFileName);
int extractAll(const char *archive_name);
int getInfo(const char *archive_name, const char mode, const int isAll, const char *fileName);
int removeElement(const char *file_name, const char *archive_name);
int checkForWrite(const char *archive_name, const char operation);
int checkForRead(const char *archive_name, const char operation);
int freeArchive(const char *archive_name);
int peekElement(const char *file_name, const char *archive_name, char *isZipped, char*newFileName);

int checkFileName_Windows(char *checking);
int checkSymbolInFileName_Windows(char checking);
void printFileAttributes(int tab, DWORD attributes);
int archiveNotWrite(char *archiveName);
void skip_input(void);
unsigned __int64 getCheckSumString(const void *ptr, size_t size);
void shiftLeftStringOne(char *s);
int strlenLPWSTR(LPCWSTR szString);
void fwriteWithCRC(const void *ptr, size_t size, FILE *f, unsigned __int64 *CRC_v);
void freadWithCRC (void *ptr, size_t size, FILE *f, unsigned __int64 *CRC_v);
void LPWSTRtoChar(const wchar_t *sz, char *result);

//Для редактирования текста в консоли
void editText(char **s);
void editText_static(char *s);
void printTextWithCursor(char *s, size_t position);
void insertInStr(char *s, size_t position, char c);
void deleteSymbol(char *s, size_t position);
