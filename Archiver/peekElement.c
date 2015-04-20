#include "HEADER/HeaderArchiver.h"

/*****************************************************************
*Взятие элемента из архива с удалением.
*
*fileName - имя файла в архиве,
*archive_name - имя архива.
*
*При ошибке возвратит значение вызываемой функции.
*
*В параметр *isZipped устанавливает, является ли файл сжатым
*			*newFileName устанавливается новое имя файла
*Возвращает значение, возвращаемое одной из используемых функций
*****************************************************************/


int peekElement(const char *file_name, const char *archive_name, char *isZipped, char *newFileName){
	int errorEx, errorRem;
	char isZip;

	errorEx = extract(file_name, archive_name, &isZip, newFileName);
	*isZipped = isZip;
	if(errorEx && errorEx != REPLACE_FILE_EXTRACT){
		return errorEx;
	}

	if(errorRem = removeElement(file_name, archive_name))
		return errorRem;
	return errorEx;
}
