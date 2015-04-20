#include "HEADER/HeaderArchiver.h"

/**********************************************************************************************************
*getInfo - метод выдаёт на поток вывода информацию о файле.
*
*archive_name - имя архива,
*mode - режим подачи данных: DENSE_INFO (d)			- плотный		(один файл - одна строка, без атрибутов);
*							 COMFORTABLY_INFO(c)	- развёрнутый	(каждое поле в отдельной строке + атрибуты).
*isAll - если истинно, то будет выведена информация обо всех файлах, иначе только о файле fileName.
*Если архив пустой, выдаст соответствующее сообщение.
**********************************************************************************************************/

#define TAB 2

int getInfo(const char *archive_name, const char mode, const int isAll, const char *fileName){
	char checksum[CHECKSUM_LENGTH + 1];
	char currentFileName[BLOCK_NAME];
	off64_t size_archive, size_file, currentSize = 0, sizeMeta;
	time_t ctime_v;
	FILE *archive;
	int lengthOfCurrentFileName = 0;
	char sym;
	char isZip;
	DWORD attributes;
	unsigned __int64 CRC_v, CRC_current_v = 0;
	char *currentSymbols[BLOCK];
	int sizeRead;
	int fileIsFound = 0;//для конкретного файла
	struct __stat64 info;
	off64_t currentSizeOfArchive = 0;
	extern current_endianness;

	archive = fopen(archive_name, "rb");
	__stat64(archive_name, &info);
	fread(checksum, 1, CHECKSUM_LENGTH+1, archive);
	
	sym = fgetc(archive);
	if(sym == END_OF_ARCHIVE){
		printf("Archive is empty.\n");
		fclose(archive);
		return 0;
	}


	while(sym != END_OF_ARCHIVE)
	{
		while((++currentSizeOfArchive <= info.st_size)&&sym && sym != END_OF_ARCHIVE){
			currentFileName[lengthOfCurrentFileName++] = sym;
			sym = fgetc(archive);
		}
		if(currentSizeOfArchive > info.st_size)
			goto archive_corrupted_gt;
		currentFileName[lengthOfCurrentFileName] = '\0';
		if(currentFileName[lengthOfCurrentFileName-1] == EMPTY_DIRECTORY)
		{
			currentFileName[lengthOfCurrentFileName-1] = '\0';
			printf("Empty directory \"%s\"\n", currentFileName);
			lengthOfCurrentFileName = 0;
			sym = fgetc(archive); 
			continue;
		}

		
/***********************************МЕТАДАННЫЕ*******************************/

		if((currentSizeOfArchive += sizeof(off64_t)) > info.st_size)
			goto archive_corrupted_gt;
		freadWithCRC(&size_archive,	sizeof(off64_t), archive, &CRC_current_v);
		if(current_endianness == ENDIANNESS_LITTLE)
			flip(&size_archive,	sizeof(off64_t));
		if((currentSizeOfArchive += size_archive) > info.st_size || size_archive < 0)
			goto archive_corrupted_gt;



		freadWithCRC(&isZip,		sizeof(char), archive, &CRC_current_v);
		freadWithCRC(&size_file,	sizeof(off64_t), archive, &CRC_current_v);
		freadWithCRC(&ctime_v,		sizeof(time_t), archive, &CRC_current_v);
		freadWithCRC(&attributes,	sizeof(DWORD), archive, &CRC_current_v);

		if(current_endianness == ENDIANNESS_LITTLE){
			flip(&size_file,	sizeof(off64_t));
			flip(&ctime_v,		sizeof(time_t));
			flip(&attributes,	sizeof(DWORD));
		}


/***************************************************************************/
		sizeMeta =	sizeof(char)//флаг сжатия
					+sizeof(off64_t)//размер исходного файла
					+sizeof(time_t)//дата создания файла
					+sizeof(DWORD)//attributes of file
					+sizeof(unsigned __int64);//CRC
		while(currentSize + BLOCK <= size_archive - sizeMeta)
			{
				freadWithCRC(currentSymbols, BLOCK, archive, &CRC_current_v);
				currentSize += BLOCK;
			}
			freadWithCRC(currentSymbols, sizeRead = size_archive - sizeMeta - currentSize, archive, &CRC_current_v);
			fread(&CRC_v, 1, sizeof(unsigned __int64), archive);
			if(current_endianness == ENDIANNESS_LITTLE)
				flip(&CRC_v, sizeof(unsigned __int64));
			
			if(CRC_current_v != CRC_v){
				printf("File \"%s\" corrupted.\n", currentFileName);
				fclose(archive);
				return ERROR_INCORRECT_CHECKSUM;
			}
			if(!isAll){
				if(strcmp(fileName, currentFileName))
					goto follow_file;
				fileIsFound = 1;
			}


			printf("%s", currentFileName);
			if(mode != DENSE_INFO){
			if(isZip)
				printf
				(
				"\n\t\tsize(archive)\t%lld bytes (file is zipped),\n"
				, size_archive
				);
			else
				printf
				(
				"\n\t\tsize(in archive)\t%lld bytes (file isn't zipped),\n"
				, size_archive
				);
			printf
				(
				"\t\tsize(file)\t%lld bytes,\n"
				"\t\tcreation date\t%s"
				, size_file, ctime(&ctime_v)
				);
			printf("\n\tAttributes:\n\n");
			printFileAttributes(TAB, attributes);
				
			}else{
				if(isZip)
					printf("\t\tsize(archive):    %lld\tbytes\t(file is zipped),   \tsize(file): %lld\tbytes\n"/*, creation date: %s"*/, size_archive, size_file/*, ctime(&ctime_v)*/);
				else
					printf("\t\tsize(in archive): %lld\tbytes\t(file isn't zipped),\tsize(file): %lld\tbytes\n"/*, creation date: %s"*/, size_archive, size_file/*, ctime(&ctime_v)*/);
			}

follow_file:
		lengthOfCurrentFileName = CRC_current_v = currentSize = 0;
		if((sym = fgetc(archive))== END_OF_ARCHIVE)
			break;
	}


	fclose(archive);
	if(!isAll && !fileIsFound)
		printf("File \"%s\" not found in archive.\n", fileName);
	return 0;

archive_corrupted_gt:
	fclose(archive);
	printf("File \"%s\" corrupted.\n", fileName);
	return ERROR_INCORRECT_CHECKSUM;
	
}
