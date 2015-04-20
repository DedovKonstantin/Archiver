#include "../HEADER/HeaderHuffman.h"

/***************************************************************************************
*Функция создаёт дерево Хаффмана по информации о символах, входящих в файл
*
*symbol_table - информация о символах
*size - размер таблицы
*
*Возвращает указатель на корень дерева или NULL, если не хватило динамической памяти
****************************************************************************************/

tree *createTree_unzip_BI(informationOfSymbol *symbol_table, int size){
	tree *result, *current;
	int position = 0;// In symbol_table
	BigInteger zero = initiliase();


	if((result = (tree *)malloc(sizeof(tree))) == NULL)
		return NULL;
	result->left = result->right = NULL;

	for(position = 0; position < size; position++)
	{
		int position_in_code = 1;

		current = result;

		for(position_in_code = 1; position_in_code <= symbol_table[position].size; position_in_code++)
		{
			if(!equal(zero, bitwiseAND((symbol_table[position].code_BI), maskPositionOne_BI(8*sizeof(BigInteger) - symbol_table[position].size + (position_in_code)))))
			{
				if(current->right == NULL)
				{
					if((current->right = (tree *)malloc(sizeof(tree))) == NULL)
						return NULL;
					current = current->right;
					current->left = current->right = NULL;
				}
				else
					current = current->right;
			}
			else
			{
				if(current->left == NULL)
				{
					if((current->left = (tree *)malloc(sizeof(tree))) == NULL)
						return NULL;
					current = current->left;
					current->left = current->right = NULL;
				}
				else
					current = current->left;
			}
		}
		current->code_BI = symbol_table[position].code_BI;
		current->symbol = symbol_table[position].symbol;
		current->size = symbol_table[position].size;
	}


	return result;
} 
