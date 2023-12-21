#include "syscall.h"
#include "copyright.h"

int main()
{    
    char c;
    PrintString("Enter a character: ");
    c = ReadChar();
    PrintChar(c);
    Halt();
}