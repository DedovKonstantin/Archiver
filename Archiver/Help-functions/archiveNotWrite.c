#include "../HEADER/HeaderArchiver.h"

//проверка на запись архива

int archiveNotWrite(char *archiveName){
	struct __stat64 st;

	if(access(archiveName, 0) == -1)//проверка на существование
		return 0;

	__stat64(archiveName, &st);
	if(!(st.st_mode & S_IREAD)||!(st.st_mode & S_IWRITE)||(access(archiveName, 2) == -1))//проверка на запись
		return 1;
	return 0;
}
	
