#include "syscall.h"
#include "copyright.h"

int main()
{    
    int number;
    PrintString("Enter a number: ");
    number = ReadInt();
    PrintInt(number);
    Halt();

    // char c;
    // PrintString("Enter a character: ");
    // c = ReadChar();
    // PrintChar(c);
    // Halt();

    // char* str;
    // int length = 255;
    // PrintString("Enter a string: ");
    // ReadString(str, 255);
    // PrintString(str);
    // Halt();
}