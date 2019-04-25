#include <stdio.h>
#include "../../module/ucGUI/Core/GUI.h"

int main(int argc, char* argv[])
{
	char c;
	printf("this is ucgui-linux test!\n");


	GUI_Init();
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);
	GUI_SetTextAlign(GUI_TA_HCENTER);
	GUI_SetFont(&GUI_Font16B_ASCII);
	GUI_DispStringAt("shuyin,i miss you",240,100);
	GUI_DrawCircle(100,100,50);//画圆
	printf("system pause\n");
	while ((c = getchar()) != '\n');
	system("pause");
	return 0;
}
