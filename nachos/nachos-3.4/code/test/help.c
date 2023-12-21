#include "syscall.h"
#include "copyright.h"

int main () {
    PrintString("\nThong tin nhom: \n");
    PrintString("     Chau Quy Duong - 21120436 \n");
    PrintString("     Vo Minh Khue - 21120486 \n");
    PrintString("     Nguyen Thi Kieu Ngan - 21120506 \n");
    PrintString("     Nguyen Thien Khanh Doan - 21120608 \n");
    PrintString("     Nguyen Huu Loc - 21120497 \n\n");
    PrintString("Nhom em co viet cac file de test nhung syscall/ham nhu sau: \n");
    PrintString("     ./test/testingSC_int.c: kiem tra system call ReadInt, PrintInt\n");
    PrintString("     ./test/testingSC_char.c: kiem tra system call ReadChar, PrintChar\n");
    PrintString("     ./test/testingSC_string.c: kiem tra system call ReadString, PrintString\n\n");
    PrintString("     ./test/ascii.c: in ra bang ma ASCII. Chuong trinh cho phep xuat ra gia tri Decimal");
    PrintString(" cua 1 ky tu va ky tu nay trong pham vi tu 32 den 126\n");
    PrintString("\t\t     vi day la vung ma console co the xuat ra ky tu ASCII co the nhin thay duoc\n\n");
    PrintString("     ./test/sort.c: cho phep nguoi dung nhap vao 1 mang cac so nguyen");
    PrintString(" va sau do sap xep tang dan gia tri mang\n");

    Halt();
}