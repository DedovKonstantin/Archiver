#include "../HEADER/HeaderArchiver.h"

//проверка имени файла на корректность

int checkFileName_Windows(char *checking){
	int i = 0;

	while(checking[i])
		if(!checkSymbolInFileName_Windows(checking[i++]))
			return 0;
	return 1;
}
