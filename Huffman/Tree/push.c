#include "../HEADER/HeaderHuffman.h"

/****************************************************************************************
*Метод добавления нового дерева tr в стек stack.
*
*Возвращает ERROR_MEMORY_DOES_NOT_ALLOCATE, если не хватило динамической памяти.
****************************************************************************************/

int push(StackTree **stack, tree *tr){
	StackTree *previous, *current, *newTree;

	if(*stack == NULL){
		if((*stack = (StackTree *)malloc(sizeof(StackTree))) == NULL)
			return ERROR_MEMORY_DOES_NOT_ALLOCATE;
		(*stack)->tr = tr;
		(*stack)->next = NULL;
		return 0;
	}

	current = *stack;
	previous = NULL;
	while(current && current->tr->weight <= tr->weight){
		previous = current;
		current = current->next;
	}

	if((newTree = (StackTree *)malloc(sizeof(StackTree))) == NULL)
		return ERROR_MEMORY_DOES_NOT_ALLOCATE;
	newTree->tr = tr;
	if(!previous){
		newTree->next = *stack;
		*stack = newTree;
		return 0;
	}
	newTree->next = current;
	previous->next = newTree;
	return 0;
}
