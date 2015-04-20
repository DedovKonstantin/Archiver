#include "../HEADER/HeaderArchiver.h"

//проверка символа в имени файла на корректность

int checkSymbolInFileName_Windows(char checking){
	switch(checking){
		/*case '\\':*///Используется для указания пути к файлу
		case '/' :
		case ':' :
		case '*' :
		case '?' :
		case '\"':
		case '<' :
		case '>' :
		case '|' : return 0;
		default  : return 1;
	}
}
