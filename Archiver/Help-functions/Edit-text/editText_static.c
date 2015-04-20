#include "../../HEADER/HeaderArchiver.h"

/*********************************************************************
Редактирование текста в консоли.
s - редактируемая строка (не более чем из MAX_PATH символов)
*********************************************************************/


void editText_static(char *s)
{
	__int64 pos = strlen(s);
	unsigned char c;

	setlocale(LC_ALL, "English");

	putchar('\r');
	printTextWithCursor(s, pos);	
	while((c = getch())!= BUTTON_ENTER)
	{
		putchar('\r');
		switch(c)
		{
		case BUTTON_ARROW_OR_DELETE:
			c = getch();
			if (c == BUTTON_LEFT && pos > 0)
				pos--;
			if (c == BUTTON_RIGHT && pos < strlen(s)&& strlen(s) < MAX_PATH)
				pos++;
			if (c == BUTTON_DELETE && pos < strlen(s))
				deleteSymbol(s, pos);
			break;
		case BUTTON_BACKSPACE:
			if(pos > 0)
				deleteSymbol(s, --pos);
			break;
		default:
			if(pos >= 0 && pos <= strlen(s) && strlen(s) < MAX_PATH)
				insertInStr(s, pos++, c);
		}
		printTextWithCursor(s, pos);	
	}
#ifdef FOR_RUSSIAN
	setlocale(LC_ALL, "Russian");
#endif
	putchar('\n');
}
