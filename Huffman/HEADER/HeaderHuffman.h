#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys\stat.h>
#include <windows.h>
#define HEADER_HUFFMAN

#define FOR_RUSSIAN

#define _CHECK
#define _CHECK_UNZIP
#define _CHECK_ZIP

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


#define OPERATION_ZIP 'z'
#define OPERATION_UNZIP 'u'

#define ARCHIVE_IS_BIGINTEGER 'B'
#define ARCHIVE_IS_NOT_BIGINTEGER 'N'

//Долже быть > CHECKSUM_LENGTH (HeaderArchive.h)
#define BLOCK 0x10000
#define BLOCK_NAME (MAX_PATH+1)


typedef struct BigInteger{
	unsigned __int64 first;
	unsigned __int64 second;
	unsigned __int64 third;
	unsigned __int64 fourth;
}BigInteger;

BigInteger shiftRight(BigInteger b, int shift);
BigInteger shiftLeft(BigInteger b, int shift);
BigInteger maskOneEnd_BI(const int size);
unsigned __int64 maskOneEnd_ULL(int size);
BigInteger bitwiseAND(const BigInteger b1, const BigInteger b2);
BigInteger bitwiseOR (const BigInteger b1, const BigInteger b2);
BigInteger initiliase(void);
BigInteger initiliase_one(void);
BigInteger maskPositionOne_BI(const int pos);
void fwrite_BI(BigInteger b, FILE *f);
void fread_BI(BigInteger *b, FILE *f);
int equal(BigInteger b1, BigInteger b2);


typedef __int64 off64_t;

typedef struct informationOfSymbol{
	unsigned char symbol;
	off64_t count;
	unsigned __int16 code;
	BigInteger code_BI;
	unsigned char size;
}informationOfSymbol;

typedef struct tree{
	off64_t weight;
	struct tree *left;
	struct tree *right;
	off64_t count;
	unsigned __int16 code;
	BigInteger code_BI;
	unsigned char size;
	unsigned char symbol;
}tree;

typedef struct StackTree{
	tree *tr;
	struct StackTree *next;
}StackTree;



void sortSymbol_table(informationOfSymbol *symbol_table, int left, int right);
tree *createTree(informationOfSymbol *begin, int size);
tree *createTree_unzip(informationOfSymbol *begin, int size);
int push(StackTree **stack, tree *tr);
tree *peek(StackTree **);
int numberTree(tree *tr, int isReset);
void createNewSymbolTable(tree *tr, informationOfSymbol *symbol_table);
void resetCounts(informationOfSymbol *symbol_table, int size);
int Huffman(const char operation, const char *file_name, const char *archive, char *sizeOfFileIsMoreThanSizeOfArchive);
void zip(FILE *archive, FILE *input, informationOfSymbol *symbol_table, const off64_t size);
void zip_BI(FILE *archive, FILE *input, informationOfSymbol *symbol_table, const off64_t size);
int unzip(FILE *archive, FILE *output);
int unzip_BI(FILE *archive, FILE *output);
unsigned __int16 maskOneEnd(const int size);
unsigned __int16 maskPositionOne(const int pos);
tree *createTree_unzip_BI(informationOfSymbol *symbol_table, int size);

//Use by archiver
void charToLPWSTR(const char *s, wchar_t *szString);

#define ENDIANNESS_BIG 0
#define ENDIANNESS_LITTLE 1
int endianness(void);

void flip(void *ptr, size_t size);
