#include "syscall.h"
#include "copyright.h"

int main()
{    
    char* str;
    int length = 255;
    PrintString("Enter a string: ");
    ReadString(str, 255);
    PrintString(str);
    Halt();
}