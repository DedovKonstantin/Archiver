#include "../HEADER/HeaderHuffman.h"

/*******************************************************
*Нумерация дерева Хаффмана, начиная с корня tr.
*Левым веткам добавляется 0, правым - 1.
*Если isReset истинно, то обнуляется значение height.
*Возвращает высоту дерева (в рёбрах)
*******************************************************/

int numberTree(tree *tr, int isReset){
	static int height = 0;

	if(isReset)
		return height = 0;
	if(tr->left){
		if(tr->size+1 > height)
			height++;
		tr->left->code = tr->code * 2;
		tr->left->code_BI = shiftLeft(tr->code_BI, 1);
		tr->left->size = tr->size + 1;
		numberTree(tr->left, 0);
	}
	if(tr->right){
		if(tr->size+1 > height)
			height++;
		tr->right->code = tr->code * 2 + 1;
		tr->right->code_BI = bitwiseOR(shiftLeft(tr->code_BI, 1), initiliase_one());
		tr->right->size = tr->size + 1;
		numberTree(tr->right, 0);
	}

	return height;
}
