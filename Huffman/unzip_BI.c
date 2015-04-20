#include "HEADER/HeaderHuffman.h"

/************************************************************************************
*unzip - метод восстановления данных из архива archive и запись их в файл output.
*Использует другой способ разархивации с восстановлением дерева.
*
*Возвращает ERROR_MEMORY_DOES_NOT_ALLOCATE при нехватке динамической памяти.
************************************************************************************/

int unzip_BI(FILE *archive, FILE *output){
	informationOfSymbol symbol_table[0x100];
	BigInteger currentSymbol; 
	unsigned __int16 generalRest, countOfDifferentSymbols;
	int i;
	int sizeOfCurrentCode;
	int position_in_current_symbol = 1;//[1..8*sizeof(unsigned __int16)]
	off64_t sizeOfFile, currentSizeOfFile = 0;
	char typeOfArchive;
	extern current_endianness;
	tree *HuffmanTree;
	tree *ptrFind;//текущее положение в дереве при поиске символа
	unsigned char currentBit;
	BigInteger zero = initiliase();


	_fseeki64(archive, 0, SEEK_SET);
	_fseeki64(output, 0, SEEK_SET);

	resetCounts(symbol_table, 0x100);
	fread(&sizeOfFile, 1, sizeof(off64_t), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&sizeOfFile, sizeof(off64_t));
	typeOfArchive = fgetc(archive);

	
	fread(&generalRest, 1, sizeof(unsigned __int16), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&generalRest, sizeof(unsigned __int16));
	fread(&countOfDifferentSymbols, 1, sizeof(unsigned __int16), archive);
	if(current_endianness == ENDIANNESS_LITTLE)
		flip(&countOfDifferentSymbols, sizeof(unsigned __int16));
	currentSizeOfFile += sizeof(unsigned char)+ sizeof(unsigned __int16) + sizeof(unsigned __int16);
	for(i = 0; i < countOfDifferentSymbols; i++){
		symbol_table[i].symbol = fgetc(archive);
		fread_BI(&symbol_table[i].code_BI, archive);
		symbol_table[i].size   = fgetc(archive);
		symbol_table[i].count = 1;

		currentSizeOfFile += sizeof(BigInteger) + 2;
	}


	sortSymbol_table(symbol_table, 0, countOfDifferentSymbols-1);
	if((HuffmanTree = createTree_unzip_BI(symbol_table, countOfDifferentSymbols)) == NULL)
		return ERROR_MEMORY_DOES_NOT_ALLOCATE;
	

 
	sizeOfCurrentCode = 0;
	fread_BI(&currentSymbol, archive);
	currentSizeOfFile += sizeof(BigInteger);

	
	
	ptrFind = HuffmanTree;
	while(currentSizeOfFile < sizeOfFile)
	{
		
		if(ptrFind->left == NULL && ptrFind->right == NULL)
		{
			fputc(ptrFind->symbol, output);
			ptrFind = HuffmanTree;
			continue;
		}
		
		currentBit = equal(zero, bitwiseAND(maskPositionOne_BI(position_in_current_symbol), currentSymbol));
		if(!currentBit)
			ptrFind = ptrFind->right;
		else
			ptrFind = ptrFind->left;
		position_in_current_symbol++;

		if(position_in_current_symbol > 8*sizeof(BigInteger))
		{
			position_in_current_symbol = 1;
			fread_BI(&currentSymbol, archive);
			currentSizeOfFile += sizeof(BigInteger);
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
		
		currentBit = equal(zero, bitwiseAND(maskPositionOne_BI(position_in_current_symbol), currentSymbol));
		if(!currentBit)
			ptrFind = ptrFind->right;
		else
			ptrFind = ptrFind->left;
		position_in_current_symbol++;
		
	}

	return 0;
}

