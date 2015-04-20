#include "../HEADER/HeaderHuffman.h"

/*****************************************************
*Метод взятия с удалением ветки дерева из стека stack.
*Возвращает указатель на верхнее дерево или 
*	NULL, если стек пустой.
*****************************************************/
tree *peek(StackTree **stack){
	tree *result;
	StackTree *del;
	
	if(!(*stack))
		return NULL;
	result = (*stack)->tr;
	del = *stack;
	*stack = (*stack)->next;
	free(del);

	return result;
}
