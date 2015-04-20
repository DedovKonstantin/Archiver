#include "HEADER/HeaderHuffman.h"

/**********************************************************************************
*zip - метод сжимает данные файла input, и записывает их в архив input.
*
*archive, input - архив, исходный файл;
*symbol_table - информация о символах в файле input;
*size - размер исходного файла в байтах.
**********************************************************************************/


void zip_BI(FILE *archive, FILE *input, informationOfSymbol *symbol_table, const off64_t size){
	BigInteger current_symbol;//данные добавляются с конца в конец
	int position_in_current_symbol = 0;//[1..256], с конца
	unsigned char sym1, sym2; 
	unsigned __int16 generalRest;//Остаток - равен количеству бит в последнем записанном символе, принадлежащих файлу.
	//В отличие от функции zip, взят unsigned __int16, на случай, если остаток будет равен 256
	
	
	unsigned __int16 countOfDifferentSymbols = 0;	//количество различных символов в файле
												//__int16 на случай, если в файле 256 различных символов
	int i,
		restInSym1;
	/*Размер полученного архива вместе с типом архивации (с BigInteger или нет), остатком
	 *и байтом с количеством различных символов.
	 */
	off64_t sizeOfFile =sizeof(unsigned char)+sizeof(unsigned __int16)+sizeof(unsigned __int16);
	
	off64_t inc;
	extern current_endianness;


	_fseeki64(archive, sizeof(off64_t) + 1 + sizeof(unsigned __int16)+sizeof(unsigned __int16), SEEK_SET);//место под память, тип архивации, остаток и количество различных символов
	for(i = 0; i < 0x100; i++){
		if(symbol_table[i].count){
			fputc(symbol_table[i].symbol, archive);
			fwrite_BI(symbol_table[i].code_BI, archive);
			fputc(symbol_table[i].size, archive);
			countOfDifferentSymbols++;
			sizeOfFile += sizeof(BigInteger) + 2;

#ifdef CHECK_ZIP
			printf("%c %hu %d\n", symbol_table[i].symbol, symbol_table[i].code, symbol_table[i].size );
#endif

		}
	}
	_fseeki64(input , 0, SEEK_SET);
	sym1 = fgetc(input);
	current_symbol = initiliase();
		
		
	for(inc = 0; inc < size;){
		if(position_in_current_symbol + symbol_table[sym1].size <= 8*sizeof(BigInteger)){
			current_symbol = shiftLeft(current_symbol, symbol_table[sym1].size);
			current_symbol = bitwiseOR(current_symbol, symbol_table[sym1].code_BI);
			position_in_current_symbol += symbol_table[sym1].size;
			sym1 = fgetc(input);
			inc++;
		}else{
			sym2 = fgetc(input);
			inc++;

			current_symbol = shiftLeft(current_symbol, 8*sizeof(BigInteger)-position_in_current_symbol);
			restInSym1 = 8*sizeof(BigInteger)-position_in_current_symbol;


			current_symbol = bitwiseOR(current_symbol, shiftRight(symbol_table[sym1].code_BI, (symbol_table[sym1].size - restInSym1)));
			fwrite_BI(current_symbol, archive);
			
			sizeOfFile += sizeof(BigInteger);

			
			
			current_symbol = bitwiseAND(symbol_table[sym1].code_BI, maskOneEnd_BI(symbol_table[sym1].size - restInSym1));
			
			position_in_current_symbol = (position_in_current_symbol + symbol_table[sym1].size)%(8*sizeof(BigInteger));
			sym1 = sym2;
		}
	}

	generalRest = position_in_current_symbol;
	
	current_symbol = shiftLeft(current_symbol, 8*sizeof(BigInteger)-position_in_current_symbol);

	fwrite_BI(current_symbol, archive);
	sizeOfFile += sizeof(BigInteger);

	_fseeki64(archive, 0, SEEK_SET);
	if(current_endianness == ENDIANNESS_LITTLE){
		flip(&sizeOfFile, sizeof(off64_t));
		flip(&generalRest, sizeof(unsigned __int16));
		flip(&countOfDifferentSymbols, sizeof(unsigned __int16));
	}
	fwrite(&sizeOfFile, 1, sizeof(off64_t), archive);
	fputc(ARCHIVE_IS_BIGINTEGER, archive);
	fwrite(&generalRest, 1, sizeof(unsigned __int16), archive);
	fwrite(&countOfDifferentSymbols, 1, sizeof(unsigned __int16), archive);
}
