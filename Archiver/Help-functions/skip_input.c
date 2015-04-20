#include "../HEADER/HeaderArchiver.h"

//пропуск введённых в консоль символов

void skip_input(void){
	char c;

	while (scanf("%c", &c) == 1 && c != '\n')
		;
}
