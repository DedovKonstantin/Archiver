#include "../HEADER/HeaderHuffman.h"

/***************************************************************************************
*Функция создаёт дерево Хаффмана по информации о символах, входящих в файл
*
*symbol_table - информация о символах
*size - размер таблицы
*
*Возвращает указатель на корень дерева или NULL, если не хватило динамической памяти
****************************************************************************************/

tree *createTree_unzip(informationOfSymbol *symbol_table, int size){
	tree *result, *current;
	int position = 0;// In symbol_table


	if((result = (tree *)malloc(sizeof(tree))) == NULL)
		return NULL;
	result->left = result->right = NULL;

	for(position = 0; position < size; position++)
	{
		int position_in_code = 1;

		current = result;

		for(position_in_code = 1; position_in_code <= symbol_table[position].size; position_in_code++)
		{
			if((symbol_table[position].code) & maskPositionOne(8*sizeof(unsigned __int16) - symbol_table[position].size + (position_in_code)))
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
		current->code = symbol_table[position].code;
		current->symbol = symbol_table[position].symbol;
		current->size = symbol_table[position].size;
	}


	return result;




} 
