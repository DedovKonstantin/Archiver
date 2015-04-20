#include "../HEADER/HeaderArchiver.h"

//Печать аттрибутов файла с отступом tab табуляций


#define PRINT_TAB(j) {\
						for(i = 0; i < j; i++)\
							putchar('\t');\
					}

void printFileAttributes(int tab, DWORD attributes){
	int i;

	if(attributes & FILE_ATTRIBUTE_ARCHIVE){//архивный файл
		PRINT_TAB(tab);
		printf("file is archive,\n");
	}
	if(attributes & FILE_ATTRIBUTE_HIDDEN){//скрытый файл
		PRINT_TAB(tab);
		printf("file is hidden,\n");
	}
	if(attributes & FILE_ATTRIBUTE_NORMAL){//У файла нет другого набора атрибутов.
		PRINT_TAB(tab);
		printf("file is normal,\n");
	}
	if(attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED){//Файл не индексируется содержащей индексацию службой.
		PRINT_TAB(tab);
		printf("file not content index,\n");
	}
	if(attributes & FILE_ATTRIBUTE_OFFLINE){//Данные файла  непосредственно не доступны. Этот атрибут указывает, что данные файла были физически перемещены на автономное запоминающее устройство. 
		PRINT_TAB(tab);
		printf("file is offline,\n");
	}
	if(attributes & FILE_ATTRIBUTE_READONLY){//Файл только для чтения. Приложения могут читать файл, но не могут записать в него или удалить его.
		PRINT_TAB(tab);
		printf("file is	read-only,\n");
	}
	if(attributes & FILE_ATTRIBUTE_REPARSE_POINT){//Файл или каталог имеет связанную точку повторной обработки.
		PRINT_TAB(tab);
		printf("file have associated reparse point,\n");
	}
	if(attributes & FILE_ATTRIBUTE_SYSTEM){//Файл - это часть операционной системы или используется исключительно ею.
		PRINT_TAB(tab);
		printf("file is system,\n");
	}
	if(attributes & FILE_ATTRIBUTE_TEMPORARY){//Файл используется для временного сохранения.
		PRINT_TAB(tab);
		printf("file is temporary,\n");
	}
}
