#include "HEADER/HeaderHuffman.h"

/************************************************************************************
*unzip - метод восстановления данных из архива archive и запись их в файл output.
*Использует способ разархивации с восстановлением дерева.
*Если тип архивации предназначен для BigInteger - произойдёт переход в unzip_BI
*
*Возвращает ERROR_MEMORY_DOES_NOT_ALLOCATE при нехватке динамической памяти.
************************************************************************************/

int unzip(FILE *archive, FILE *output){
	informationOfSymbol symbol_table[0x100];
	unsigned __int16 currentSymbol, countOfDifferentSymbols;
	unsigned char generalRest;
	int i;
	int position_in_current_symbol = 1;//[1..8*sizeof(unsigned __int16)]
	off64_t sizeOfFile, currentSizeOfFile = 0;
	char typeOfArchive;
	extern current_endianness;
	tree *HuffmanTree;
	tree *ptrFind;//текущее положение в дереве при поиске символа
	unsigned __int16 currentBit;


	_fseeki64(archive, 0, SEEK_SET);
	_fseeki64(output, 0, SEEK_SET);

	resetCounts(symbol_table, 0x100);
	fread(&sizeOfFile, 1, sizeof(off64_t), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&sizeOfFile, sizeof(off64_t));
	typeOfArchive = fgetc(archive);
	if(typeOfArchive == ARCHIVE_IS_BIGINTEGER){
		if((unzip_BI(archive, output)) == ERROR_MEMORY_DOES_NOT_ALLOCATE)
			return ERROR_MEMORY_DOES_NOT_ALLOCATE; 
		return 0;
	}

	
	generalRest = fgetc(archive);
	fread(&countOfDifferentSymbols, 1, sizeof(unsigned __int16), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&countOfDifferentSymbols, sizeof(unsigned __int16));
	currentSizeOfFile += sizeof(unsigned char)+ sizeof(unsigned char) + sizeof(unsigned __int16);
	for(i = 0; i < countOfDifferentSymbols; i++){
		symbol_table[i].symbol = fgetc(archive);
		fread(&symbol_table[i].code, 1, sizeof(unsigned __int16), archive);
		if(current_endianness == ENDIANNESS_LITTLE)
			flip(&symbol_table[i].code, sizeof(unsigned __int16));
		symbol_table[i].size   = fgetc(archive);
		symbol_table[i].count = 1;

		currentSizeOfFile += sizeof(unsigned __int16) + 2;
	}

#ifdef CHECK_UNZIP
	for(i = 0; i < countOfDifferentSymbols; i++)
		printf("%c %hu %d\n", symbol_table[i].symbol, symbol_table[i].code, symbol_table[i].size );
#endif

	sortSymbol_table(symbol_table, 0, countOfDifferentSymbols-1);
	if((HuffmanTree = createTree_unzip(symbol_table, countOfDifferentSymbols)) == NULL)
		return ERROR_MEMORY_DOES_NOT_ALLOCATE;
	

 

	fread(&currentSymbol, 1, sizeof(unsigned __int16), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&currentSymbol, sizeof(unsigned __int16));
	currentSizeOfFile += sizeof(unsigned __int16);

	
	
	ptrFind = HuffmanTree;
	while(currentSizeOfFile < sizeOfFile)
	{
		
		if(ptrFind->left == NULL && ptrFind->right == NULL)
		{
			fputc(ptrFind->symbol, output);
			ptrFind = HuffmanTree;
			continue;
		}
		
		currentBit = (maskPositionOne(position_in_current_symbol) & currentSymbol);
		if(currentBit)
			ptrFind = ptrFind->right;
		else
			ptrFind = ptrFind->left;
		position_in_current_symbol++;

		if(position_in_current_symbol > 8*sizeof(unsigned __int16))
		{
			position_in_current_symbol = 1;
			fread(&currentSymbol, 1, sizeof(unsigned __int16), archive);
			if(current_endianness == ENDIANNESS_LITTLE)
				flip(&currentSymbol, sizeof(unsigned __int16));
			currentSizeOfFile += sizeof(unsigned __int16);
			continue;
		}
	}


	while(position_in_current_symbol <= generalRest+1)
	{


		if(ptrFind->left == NULL && ptrFind->right == NULL)
		{
			fputc(ptrFind->symbol, output);
			ptrFind = HuffmanTree;
			continue;
		}
		
		currentBit = (maskPositionOne(position_in_current_symbol) & currentSymbol);
		if(currentBit)
			ptrFind = ptrFind->right;
		else
			ptrFind = ptrFind->left;
		position_in_current_symbol++;
	}

	return 0;
}

