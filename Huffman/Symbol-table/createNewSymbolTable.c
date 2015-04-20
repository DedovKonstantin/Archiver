#include "../HEADER/HeaderHuffman.h"

/*************************************************************************************
*Метод создания новой таблицы символов symbol_table по дереву Хаффмана tr.
*В результате каждый символ будет иметь индекс со своим int-номером.
*************************************************************************************/
 
void createNewSymbolTable(tree *tr, informationOfSymbol *symbol_table){

	int flag = 0;

	if(tr->right != NULL){
		createNewSymbolTable(tr->right, symbol_table);
		flag++;
	}
	if(tr->left != NULL){
		createNewSymbolTable(tr->left, symbol_table);
		flag++;
	}
	if( !flag){
		
		symbol_table[tr->symbol].code   = tr->code;
		symbol_table[tr->symbol].code_BI= tr->code_BI;
		symbol_table[tr->symbol].size   = tr->size;
		symbol_table[tr->symbol].symbol = tr->symbol;
		symbol_table[tr->symbol].count  = tr->count;
		
	}


}
