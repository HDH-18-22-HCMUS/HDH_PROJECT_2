#include "syscall.h"
#include "copyright.h"
int main()
{
    int count;
    for (count = 1; count < 128; count++)
    {
        PrintInt(count);
        PrintChar(' ');
        PrintChar((char)count);
        PrintString("\n");
    }
}