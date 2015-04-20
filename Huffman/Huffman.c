#include "HEADER/HeaderHuffman.h"

/****************************************************************************************************
*Huffman - функция сжатия/восстановления сжатого файла по алгоритму Хаффмана.
*
*operation - операция: сжатие(OPERATION_ZIP) или восстановление(OPERATION_UNZIP);
*file_name - имя файла, в котором будут записаны(из которого берутся) восстановленные данные;
*archive	   - имя архива, в который записываются (из которого берутся) сжатые данные.
*В sizeOfFileIsMoreThanSizeOfArchive устанавливает, был ли файл сжат.
*При разархивации атрибуты файла становятся такими же, как у архива.
*
*Возвращает ERROR_MEMORY_DOES_NOT_ALLOCATE при нехватке динамической памяти.
*			ERROR_FILE_NOT_FOUND_IN_DIRECTORY, если файл с именем file_name не был найден.
****************************************************************************************************/


int Huffman(const char operation, const char *file_name, const char *archive, char *sizeOfFileIsMoreThanSizeOfArchive){
	FILE *input, *output, *unzipFile;
	informationOfSymbol symbol_table[0x100];
	off64_t symbols_count[0x100];
	unsigned char current_symbol;
	int i, count_empty;
	tree *createdTree;
	struct __stat64 informationAboutFile, informationAboutArchive;
	off64_t inc;
	int height;
	wchar_t archiveLPWSTR[BLOCK_NAME], file_nameLPWSTR[BLOCK_NAME];
	DWORD attributes;
	extern current_endianness;
	int result;//Возвращаемое значение


	memset(symbols_count, 0, sizeof(off64_t)*0x100);

	if(operation == OPERATION_ZIP){
		if((input = fopen(file_name, "rb")) == NULL)
			return ERROR_FILE_NOT_FOUND_IN_DIRECTORY;
		output = fopen(archive, "wb");
		__stat64(file_name, &informationAboutFile);
		
		for(inc = 0; inc < informationAboutFile.st_size; inc++){
			current_symbol = fgetc(input);
			symbols_count[current_symbol]++;
		}
		for(i = 0; i < 0x100; i++){
			symbol_table[i].symbol = i;
			symbol_table[i].count = symbols_count[i];
		}
		sortSymbol_table(symbol_table, 0, 0x100-1);
		count_empty = 0;
		while(!(symbol_table[count_empty].count))
			count_empty++;
		if((createdTree = createTree(&symbol_table[count_empty], 0x100 - count_empty)) == NULL)
			return ERROR_MEMORY_DOES_NOT_ALLOCATE;

		height = numberTree(createdTree, 0);
		numberTree(createdTree, 1);
		resetCounts(symbol_table, 0x100);
		createNewSymbolTable(createdTree, symbol_table);
		
		
		if(height <= 8*sizeof(unsigned __int16))
			zip(output, input, symbol_table, informationAboutFile.st_size);
		else
			zip_BI(output, input, symbol_table, informationAboutFile.st_size);

		fclose(input); 
		fclose(output);
		__stat64(archive, &informationAboutArchive);
		if(informationAboutFile.st_size > informationAboutArchive.st_size)
			*sizeOfFileIsMoreThanSizeOfArchive = 1;
		else
			*sizeOfFileIsMoreThanSizeOfArchive = 0;

		return 0;
	}
	else if(operation == OPERATION_UNZIP){

		output = fopen(archive, "rb");
		unzipFile = fopen(file_name, "wb");

		charToLPWSTR(archive, archiveLPWSTR);
		attributes = GetFileAttributesW(archiveLPWSTR);
		result = unzip(output, unzipFile);
		charToLPWSTR(file_name, file_nameLPWSTR);
		SetFileAttributesW(file_nameLPWSTR, attributes);
		fclose(output);
		fclose(unzipFile);
		return result;
	}


	/*Сюда не доходит*/
	return 0;

}
