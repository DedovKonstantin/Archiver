#include "HEADER/HeaderHuffman.h"

/**********************************************************************************
*zip - метод сжимает данные файла input, и записывает их в архив input.
*
*archive, input - архив, исходный файл;
*symbol_table - информация о символах в файле input;
*size - размер исходного файла в байтах.
**********************************************************************************/


void zip(FILE *archive, FILE *input, informationOfSymbol *symbol_table, const off64_t size){
	unsigned __int16 current_symbol;//данные добавляются с конца в конец
	int position_in_current_symbol = 0;//[1..8*sizeof(unsigned __int16)], с конца
	unsigned char sym1, sym2, 
		generalRest;//Остаток - равен количеству бит в последнем записанном символе, принадлежащих файлу.
	unsigned __int16 countOfDifferentSymbols = 0;	//количество различных символов в файле
												//__int16 на случай, если  в файле 256 различных символов
	int i,
		restInSym1;

	/*Размер полученного архива вместе с типом архивации (с BigInteger или нет), остатком
	 *и байтом с количеством различных символов.
	 */
	off64_t sizeOfFile = sizeof(unsigned char)+sizeof(unsigned char)+sizeof(unsigned __int16);
	
	off64_t inc;
	extern current_endianness;


	_fseeki64(archive, sizeof(off64_t) + 1 + sizeof(unsigned __int16)+1, SEEK_SET);//место под память, тип архивации, остаток и количество различных символов
	for(i = 0; i < 0x100; i++){
		if(symbol_table[i].count){
			fputc(symbol_table[i].symbol, archive);
			
			if(current_endianness == ENDIANNESS_LITTLE)
				flip(&symbol_table[i].code, sizeof(unsigned __int16));
			
			fwrite(&symbol_table[i].code, 1, sizeof(unsigned __int16), archive);
			
			if(current_endianness == ENDIANNESS_LITTLE)
				flip(&symbol_table[i].code, sizeof(unsigned __int16));

			fputc(symbol_table[i].size, archive);
			countOfDifferentSymbols++;
			sizeOfFile += sizeof(unsigned __int16) + 2;

#ifdef CHECK_ZIP
			printf("%c %hu %d\n", symbol_table[i].symbol, symbol_table[i].code, symbol_table[i].size );
#endif

		}
	}
	_fseeki64(input , 0, SEEK_SET);
	sym1 = fgetc(input);
	current_symbol = 0;
		
		
	for(inc = 0; inc < size;){
		if(position_in_current_symbol + symbol_table[sym1].size <= 8*sizeof(unsigned __int16)){
			for(i = 0; i < symbol_table[sym1].size; i++)
				current_symbol *= 2;
			current_symbol += symbol_table[sym1].code;
			position_in_current_symbol += symbol_table[sym1].size;
			sym1 = fgetc(input);
			inc++;
		}else{
			sym2 = fgetc(input);
			inc++;
			restInSym1 = 0;
			for(i = position_in_current_symbol; i < 8*sizeof(unsigned __int16); i++){
				current_symbol *= 2;
				restInSym1++;
			}
			current_symbol += symbol_table[sym1].code >> (symbol_table[sym1].size - restInSym1);

			if(current_endianness == ENDIANNESS_LITTLE)
				flip(&current_symbol, sizeof(unsigned __int16));
			fwrite(&current_symbol, 1, sizeof(unsigned __int16), archive);
			
			sizeOfFile += sizeof(unsigned __int16);

			
			
			current_symbol = symbol_table[sym1].code & maskOneEnd(symbol_table[sym1].size - restInSym1);
			
			position_in_current_symbol = (position_in_current_symbol + symbol_table[sym1].size)%(8*sizeof(unsigned __int16));
			sym1 = sym2;
		}
	}

	generalRest = position_in_current_symbol;
	for(i = 0; i < 8*sizeof(unsigned __int16) - position_in_current_symbol; i++)
		current_symbol *= 2;

	if(current_endianness == ENDIANNESS_LITTLE){
		flip(&current_symbol, sizeof(unsigned __int16));
		flip(&countOfDifferentSymbols, sizeof(unsigned __int16));
	}
	fwrite(&current_symbol, 1, sizeof(unsigned __int16), archive);
	sizeOfFile += sizeof(unsigned __int16);
	if(current_endianness == ENDIANNESS_LITTLE){
		flip(&sizeOfFile, sizeof(off64_t));
	}
	_fseeki64(archive, 0, SEEK_SET);
	fwrite(&sizeOfFile, 1, sizeof(off64_t), archive);
	fputc(ARCHIVE_IS_NOT_BIGINTEGER, archive);
	fputc(generalRest, archive);
	fwrite(&countOfDifferentSymbols, 1, sizeof(unsigned __int16), archive);
}
