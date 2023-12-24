#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 32

int main()
{
    // Biến lưu ID file và lưu tên của file cần tạo.
	char filename[MAX_LENGTH];
    int fileID;

    PrintString("\n\r -PROGRAM TEST CREATE FILE- \r\n");

	// Thông báo nhập xuất tên file mình sẽ tạo.
	PrintString("Input name of file you want to create: ");
	ReadString(filename, MAX_LENGTH);

	// Lấy trạng thái/ID của file đã tạo.
	fileID = CreateFile(filename);

	// Kiểm tra xem file đã tạo xong thành công hay không.
	if(fileID == 1)
	{
		PrintString("Cannot create!\n");
		Halt();
	}
	else
	{
		PrintString("Create successfully\n!");
		Halt();
	}

	PrintString("\n\r -PROGRAM TEST CREATE FILE WAS DONE- \r\n");
	Halt();
}