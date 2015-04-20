#include "../../HEADER/HeaderArchiver.h"

/**********************************************************************************
Печать строки s с добавлением перед символом с позицией position символа SEPARATOR
**********************************************************************************/

void printTextWithCursor(char *s, size_t position)
{
	size_t currPos, length = strlen(s);
	WORD wColour;
	HANDLE hSTDOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO SCRN_INFO;
    GetConsoleScreenBufferInfo(hSTDOut, &SCRN_INFO);


	for(currPos = 0; currPos <= length; currPos++)
	{
		if(currPos < position)
			putchar(s[currPos]);
		else if(currPos != position)
			putchar(s[currPos-1]);
		else
		{
			wColour = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			SetConsoleTextAttribute(hSTDOut,wColour);
			putchar(SEPARATOR);
			wColour = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			SetConsoleTextAttribute(hSTDOut,wColour);
		}
	}
	putchar('\0');
}
