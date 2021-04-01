#include <stdlib.h>
#include "test.h"
#define DEF def

//行注释
//line comment
void funcDeclare(int i, float f);
void funcVoid();
double globVar = 1, globarr[9];
/*
 *block comment
 * 
 */
int main()
{
    int i_1 = 123ul, i_2 = 0x9b7c, i_3 = 0125;
    float f_1 = .1l, f_2 = .3e4;

    double d = .314e3;
    char c_1 = '\0x8';

    char s[0x19] = "May there be no course design in heaven";
    if (c_1 == 's' || i_1 != i_2)
    {
        funcDeclare(i, 0.1);
        if (c_1 == 9)
        {
            globVar *= 2;
        }
    }
    else if (d == .27 || c_1 > '9')
    {
        funcDeclare(i, 0.1);
    }
    else
    {
        i_1 += 0;
    }
    while (i_1 >= 0 && f_1 < 1 + 3)
    {
        for (i_1 = 0; i_1 <= c_1; i_1 += 1)
        {
            i_1 += 0;
            continue;
        }
        break;
    }
    //    i_1=f_1%98*((8 - 2)+(9-i_3)/4);
    return i_1;
}
