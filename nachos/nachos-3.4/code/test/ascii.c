#include "syscall.h"
#include "copyright.h"
#define MAX_ASCII 126
#define MIN_ASCII 32

int main () {
    int i;
	PrintString("Bang ma ASCII cho cac ki tu co the nhin thay duoc (tu khoang 32 den 126): \n");
	for (i = MIN_ASCII; i <= MAX_ASCII; i++) {
		PrintInt(i);
		PrintChar(':');
		PrintChar((char)i);
		PrintChar('\n');
	}
    Halt();
}