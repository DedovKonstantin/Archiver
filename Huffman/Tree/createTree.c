#include "../HEADER/HeaderHuffman.h"

/***************************************************************************************
*Функция создаёт дерево Хаффмана по информации о символах, входящих в файл
*
*symbol_table - информация о символах
*size - размер таблицы
*
*Возвращает указатель на корень дерева или NULL, если не хватило динамической памяти
****************************************************************************************/
//symbol_table отсортирована по count

tree *createTree(informationOfSymbol *symbol_table, int size){
	tree *trees, *current1, *current2, **node;
	StackTree *head = NULL;
	int i;
	int countOfNodes = 0, currentPositionOfNodes = 0;

	
	if((node = (tree **)malloc(sizeof(tree *))) == NULL)
		return NULL;

	if((trees = (tree *)malloc(sizeof(tree) * size)) == NULL)
		return NULL;
	for(i = 0; i < size; i++){
		trees[i].weight = symbol_table[i].count;
		trees[i].symbol = symbol_table[i].symbol;
		trees[i].count = symbol_table[i].count;
		trees[i].left = trees[i].right = NULL;
		trees[i].size = 0;
		trees[i].code = 0;
		trees[i].code_BI = initiliase();
		push(&head, &trees[i]);
	}
	current1 = peek(&head);
	current2 = peek(&head);
	while(head){
		
		if((node = (tree **)realloc(node, sizeof(tree *)*(countOfNodes+1))) == NULL)
			return NULL;
		if((node[countOfNodes] = (tree *)malloc(sizeof(tree))) == NULL)
			return NULL;
		node[countOfNodes]->left = current1;
		node[countOfNodes]->right = current2;
		node[countOfNodes]->size = node[countOfNodes]->code = 0;
		node[countOfNodes]->code_BI = initiliase();
		if(current2)
			node[countOfNodes]->weight = current1->weight + current2->weight;
		else
			node[countOfNodes]->weight = current1->weight;
		if((push(&head, node[countOfNodes])) == ERROR_MEMORY_DOES_NOT_ALLOCATE)
			return NULL;
		current1 = peek(&head);
		current2 = peek(&head);
		countOfNodes++;
	}

	if((node = (tree **)realloc(node, sizeof(tree *)*(countOfNodes+1))) == NULL)
		return NULL;
	if((node[countOfNodes] = (tree *)malloc(sizeof(tree))) == NULL)
		return NULL;
	node[countOfNodes]->left = current1;
	node[countOfNodes]->right = current2;
	if(current2)
		node[countOfNodes]->weight = current1->weight + current2->weight;
	else if(current1)
		node[countOfNodes]->weight = current1->weight;
	node[countOfNodes]->size = node[countOfNodes]->code = 0;
	node[countOfNodes]->code_BI = initiliase();

	if((push(&head, node[countOfNodes])) == ERROR_MEMORY_DOES_NOT_ALLOCATE)
		return NULL;

	return node[countOfNodes];
} 
