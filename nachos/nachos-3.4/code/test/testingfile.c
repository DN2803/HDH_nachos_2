#include "syscall.h"

#define STDIN 0
#define STDOUT 1

int main() {
    int check = Open("myfile.txt", 0);

    if (check != -1)
    {
        PrintString("Open file successfully!\n");

        if (Write("Kamisato Ryuu: Soumetsu\n", 25, check) != -1)
        {
            PrintString("Write successfully");
        } 
        else
        {
            PrintString("Something went wrong...\n");
        }

        Close(check);
    }
    else
    {
        PrintString("Cannot open this file!\n");
    }
    

	Halt();
}
