#include <stdio.h>
#include <unistd.h>
#include "../../module/touchScreen/touchScreen.h"

int main()
    {
    int ret;
    int x,y;
    ret = TouchScreenInit();
    if ( ret < 0 )
        {
        printf("TouchScreenInit\n");
        return -1;
        }

    TouchScreenInfo();

    while(1)
    {
    ret = TouchScreenRead(&x,&y);
    if ( ret == 0)
        {
        printf("Press X = %d ,Press Y = %d\n",x,y);
        }
    }

    TouchScreenClose();
    return 0;
    }
