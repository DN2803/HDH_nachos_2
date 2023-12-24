// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void increasePC()
{
    int counter = machine->ReadRegister(PCReg);
   	machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
   	machine->WriteRegister(NextPCReg, counter + 4);
}

void reverseString(char array[], int start, int end) {
    while (start < end){
        char temp = array[start];
        array[start] = array[end];
        array[end] = temp;
        start++;
        end--;
    }
}

// Input: - User space address (int)
//- Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	//printf("\n Filename u2s:");
	for (i = 0 ; i < limit ;i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char) oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: - User space address (int)
//- Limit of buffer (int)
//- Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do {
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i, 1, oneChar);
		i++;
	}while (i < len && oneChar != 0);

	return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);


    switch (which)
    {
    case NoException:
        return;
    case PageFaultException: 
        DEBUG('a', "\n Shutdown, No valid translation found.");
        printf("\n\nShut down, No valid translation found.");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "\n Shutdown, Write attempted to page marked read-only.");
        printf("\n\nShutdown, Write attempted to page marked read-only.");
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "\n Shutdown, Translation resulted in an invalid physical address.");
        printf("\n\nShut down, Translation resulted in an invalid physical address.");
        interrupt->Halt();
        break;
    case AddressErrorException:
        DEBUG('a', "\n Shutdown, Unaligned reference or one that was beyond the end of the address space.");
        printf("\n\nShut down, Unaligned reference or one that was beyond the end of the address space.");
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "\n Shutdown, Integer overflow in add or sub.");
        printf("\n\nShut down, Integer overflow in add or sub.");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "\n Shutdown, Unimplemented or reserved instr..");
        printf("\n\nShut down, Unimplemented or reserved instr..");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "\n");
        printf("\n\n");
        interrupt->Halt();
        break;
    case SyscallException:
        // cần xử lý nè 
        switch (type)
        {
            case SC_Halt: 
                DEBUG('a', "\n Shutdown, initiated by user program"); 
                printf ("\n\n Shutdown, initiated by user program"); 
                interrupt->Halt(); 
                return; 

            //System call đọc 1 số nguyên
            case SC_ReadInt: 
            {
                int maxLength = 255;
                char* buf = new char[maxLength + 1];
                bool isNegative = false;
                int startIndex = 0;
                int ans = 0, size = 0, countMinus = 0;

				size = synchcons->Read(buf, maxLength);

                if (size == 0)
                {
                    DEBUG('a', "\n Invalid input number!");
					printf("\n\n Invalid input number!");

                    machine->WriteRegister(2, 0);
                    increasePC();
                    delete buf;
                    return;
                }

                // if (buf[0] == '-')
                // {
                //    isNegative = true;
                //    startIndex = 1;
                // }
                
                // VD: nhập số ---1234 => Là số -1234 (số âm)
                // VD: nhập số --1234 =>  Là số 1234 (số dương)
                for (int i = 0; i < size; i++)
                    if (buf[i] == '-')
                        countMinus++;
                startIndex = countMinus;
                if (countMinus % 2 != 0)
                    isNegative = true;

                for (int i = startIndex; i < size; i++)
                {
                    // Tồn tại phần tử không là 1 chữ số => Báo lỗi
                    if (buf[i] < '0' || buf[i] > '9')
                    {
                        DEBUG('a', "\n Invalid input number!");
					    printf("\n\n Invalid input number!");

                        machine->WriteRegister(2, 0);
                        increasePC();
                        delete buf;
                        return;
                    }
                    // VD: 10.00 thì in ra 10
                    // Có chữ số nào khác 0 tồn tại sau phần thập phân => Báo lỗi
                    else if (buf[i] == '.')
                    {
                        //////////////// Lát cải tiến sau
                        for (int j = i + 1; j < size; j++)
                        if (buf[j] != '0')
                        {
                            DEBUG('a', "\n Invalid input number!");
                            printf("\n\n Invalid input number!");
                            ans = 0;
                        }

                        machine->WriteRegister(2, ans);
                        increasePC();
                        delete buf;
                        return;
                    }
                    else
                        ans = ans * 10 + (buf[i] - '0');
                }

                if (isNegative)
                    ans *= -1;
                
                machine->WriteRegister(2, ans);
				increasePC();
				delete buf;
				return;   
            }

            // System call in 1 số nguyên
            case SC_PrintInt:
            {
                int number = machine->ReadRegister(4);

                // Trường hợp là số 0
                if (number == 0)
                {
                    synchcons->Write("0", 1);
					increasePC();
					break;
                }

                int maxLength = 255;
                char* buf = new char[maxLength + 1];
                bool isNegative = false;
                int startIndex = 0;
                
                if (number < 0)
                {
                    isNegative = true;
                    number *= -1; // Chuyển qua số dương cho dễ xử lý

                    buf[startIndex++] = '-';
                }

                while (number != 0)
                {
                    buf[startIndex] = number % 10 + '0';
                    number /= 10;
                    startIndex++;
                }
                buf[startIndex] = '\0';

                // Đảo chuỗi
                if (isNegative)
                    reverseString(buf, 1, startIndex - 1);
                else
                    reverseString(buf, 0, startIndex - 1);
                
                synchcons->Write(buf, startIndex);
                increasePC();
                break;
            }

            // System call đọc 1 ký tự
            case SC_ReadChar:
            {
                int maxLength = 255;
                char* buf = new char[255];
                int size = synchcons->Read(buf, maxLength);

                // Nếu như nhâp vào nhiều hơn 1 ký tự thì báo lỗi
                if (size > 1)
                {
                    printf("Invalid character!");
                    DEBUG('a', "\nERROR: Invalid character!");
                    machine->WriteRegister(2, 0);
                }
                // Người dùng không nhập gì thì báo lỗi
                else if (size == 0)
                {
                    printf("Empty input!");
					DEBUG('a', "\nERROR: Empty input!");
					machine->WriteRegister(2, 0);
                }
                else
                    machine->WriteRegister(2, buf[0]);
                
                increasePC();
                delete buf;
                break;
            }

            // System call xuất 1 ký tự
            case SC_PrintChar:
            {
                char c = (char) machine->ReadRegister(4);

                synchcons->Write(&c, 1);
                increasePC();
                break;
            }

            // System call đọc 1 chuỗi
            case SC_ReadString:
            {
                int address = machine->ReadRegister(4);
                int size = machine->ReadRegister(5);

                // Nhập vào chuỗi ký tự không hợp lệ thì tắt HĐH
                if (size < 1)
                {
                    interrupt->Halt();
                    return;
                }

                char* buf = new char[size + 1];
                int szCheck = synchcons->Read(buf, size);

                // Nếu không đọc được độ dài chuỗi => tắt HĐH
                if (szCheck == -1)
                {
                    interrupt->Halt();
                    return;
                }
                if (szCheck == 0)
                    break;

                buf[szCheck] = '\0';
                System2User(address, size + 1, buf);

                increasePC();
                delete buf;
                break;
            }

            // System call xuất 1 chuỗi
            case SC_PrintString:
            {
                int address;
                char* buf;

                address = machine->ReadRegister(4);
                buf = User2System(address, 255);

                // Lấy độ dài thực sự của chuỗi nhập vào
                int actualSize = 0;
                while (buf[actualSize] != '\0')
                    actualSize++;
                
                synchcons->Write(buf, actualSize + 1);

                delete buf;
                increasePC();
                break;
            }

            // System call tạo file
            case SC_CreateFile:
		    {
                // Input: Địa chỉ từ vùng nhớ user của tên file
                // Output: -1 = Lỗi, 0 = Thành công
                // Chức năng: Tạo ra file với tham số là tên file
                int virtAddr;
                char* filename;
                DEBUG('a', "\n SC_CreateFile call ...");
                DEBUG('a', "\n Reading virtual address of filename");
                
                // Đọc địa chỉ của file từ thanh ghi r4
                virtAddr = machine->ReadRegister(4); 
                DEBUG('a', "\n Reading filename.");
                
                // Sao chép không gian bộ nhớ User sang System, với độ dài max: 32 + 1 = 33 bytes
                filename = User2System(virtAddr, MaxFileLength + 1);
                if (strlen(filename) == 0)
                {
                    printf("\n File name is not valid");
                    DEBUG('a', "\n File name is not valid");
                    machine->WriteRegister(2, -1); //Return -1 vào thanh ghi r2
                    increasePC();
                    break;
                }
                
                if (filename == NULL)  // Trường hợp không đọc được
                {
                    printf("\n Not enough memory in system");
                    DEBUG('a', "\n Not enough memory in system");
                    machine->WriteRegister(2, -1); //Return -1 vào thanh ghi r2
                    delete filename;
                    increasePC();                    
                    break;
                }
                DEBUG('a', "\n Finished reading filename");
                
                if (!fileSystem->Create(filename, 0)) // Tạo file bằng hàm Create của fileSystem, tra ve ket qua
                {
                    // Thất bại
                    printf("\n Error create file '%s'", filename);
                    machine->WriteRegister(2, -1);
                    delete filename;
                    increasePC();
                    break;
                }
                
                // Thành công
                machine->WriteRegister(2, 0);
                delete filename;
                increasePC();
                break;
            }

            case SC_Open:
            {
                int virtAddr = machine->ReadRegister(4); 
                int type = machine->ReadRegister(5); 
                char* filename;
                filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
                
                int freeSlot = fileSystem->FindFreeSlot(); //Tìm kiếm slot trống

                if (freeSlot != -1) // Xử lý khi còn slot trống
                {
                    if (type == 0 || type == 1) // Chỉ xử lý khi type = 0 hoặc 1
                    {

                        if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
                        {
                            machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
                        }
                    }
                    else if (type == 2) // Xử lý stdin, type quy ước là 2
                    {
                        machine->WriteRegister(2, 0); // Trả về OpenFileID
                    }
                    else if (type == 3) // Xử lý stdout, type quy ước là 3
                    {
                        machine->WriteRegister(2, 1); // Trả về OpenFileID
                    }
                    else
                    {
                        machine->WriteRegister(2, -1); // Báo lỗi khi type không hợp lệ
                    }
                    increasePC();
                    delete[] filename;
                    break;
                }
                
                machine->WriteRegister(2, -1); // Không mở được file, return -1
                increasePC();                
                delete[] filename;
                break;
            }

            case SC_Close:
            {
                // Input: id của file(OpenFileID)
                // Output: 0: thành công, -1 thất bại
                int fid = machine->ReadRegister(4); // Lấy id của file
                if (fid >= 0 && fid <= 9) // xử lý khi fid nằm trong [0, 14]
                {
                    if (fileSystem->openf[fid]) // Thành công
                    {
                        delete fileSystem->openf[fid]; // Xóa vùng nhớ lưu trữ file
                        fileSystem->openf[fid] = NULL; // Gán vùng nhớ cho NULL
                        machine->WriteRegister(2, 0);
                        break;
                    }
                }
                machine->WriteRegister(2, -1);
                increasePC();
                break;
            }

            case SC_Read:
            {
                int virtAddr = machine->ReadRegister(4); // Địa chỉ của tham số buffer
                int charcount = machine->ReadRegister(5); // Kích thước
                int id = machine->ReadRegister(6); // ID của file 
                int OldPos;
                int NewPos;
                char *buf;

                // Kiểm tra ID của file truyền vào có nằm ngoài bảng mô tả file hay không
                if (id < 0 || id > 9)
                {
                    printf("\nFile's ID is not valid!.");
                    machine->WriteRegister(2, -1);
                    increasePC();
                    return;
                }
                // Kiểm tra file có tồn tại hay không
                if (fileSystem->openf[id] == NULL)
                {
                    printf("\nFile doesn't exist!");
                    machine->WriteRegister(2, -1);
                    increasePC();
                    return;
                }
                // Trường hợp đọc file stdout thì trả về -1 (quy ước type là 3)
                if (fileSystem->openf[id]->type == 3) 
                {
                    printf("\n[Stdout] file cannot be read!");
                    machine->WriteRegister(2, -1);
                    increasePC();
                    return;
                }

                OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiểm tra thành công thì lấy vị trí OldPos
                buf = User2System(virtAddr, charcount);
                // Xét trường hợp đọc file stdin
                if (fileSystem->openf[id]->type == 2)
                {
                    // Sử dụng hàm Read của lớp SynchConsole để trả về số byte thật sự đọc được
                    int size = synchcons->Read(buf, charcount); 
                    System2User(virtAddr, size, buf); 
                    machine->WriteRegister(2, size); // Trả về số byte thực sự đọc được
                    delete[] buf;
                    increasePC();
                    return;
                }
                // Trường hợp đọc file bình thường
                if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
                {
                    // Số byte thật sự = NewPos - OldPos
                    NewPos = fileSystem->openf[id]->GetCurrentPos();

                    System2User(virtAddr, NewPos - OldPos, buf); 
                    machine->WriteRegister(2, NewPos - OldPos);
                }
                else
                {
                    // Đọc file có nội dung là NULL
                    machine->WriteRegister(2, -2);
                }
                delete[] buf;
                increasePC();
                return;
            }
            case SC_Write:
            {
                int virtAddr = machine->ReadRegister(4); // Địa chỉ của tham số buffer
                int charcount = machine->ReadRegister(5); // Kích thước
                int id = machine->ReadRegister(6); // ID của file
                int OldPos;
                int NewPos;
                char *buf;
                // Kiểm tra ID của file
                if (id < 0 || id > 9)
                {
                    printf("\nFile's ID is not valid");
                    machine->WriteRegister(2, -1);
                    increasePC();
                    return;
                }
                // File có tồn tại hay không
                if (fileSystem->openf[id] == NULL)
                {
                    printf("\nFile does not exist!");
                    machine->WriteRegister(2, -1);
                    increasePC();
                    return;
                }
                // Trường hợp ghi file stdin hoặc file read-only
                if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
                {
                    printf("\nCannot write on stdin file or read-only file!");
                    machine->WriteRegister(2, -1);
                    increasePC();
                    return;
                }

                // Trường hợp ghi file stdout
                if (fileSystem->openf[id]->type == 3) 
                {
                    int i = 0;
                    while (buf[i] != 0 && buf[i] != '\n')
                    {
                        synchcons->Write(buf + i, 1); // Hàm Write của lớp SynchConsole
                        i++;
                    }
                    buf[i] = '\n';
                    synchcons->Write(buf + i, 1); // Write ký tự \n
                    machine->WriteRegister(2, i - 1); // Trả về số byte thực sự write được
                    delete[] buf;
                    increasePC();
                    return;
                }
                
                OldPos = fileSystem->openf[id]->GetCurrentPos(); // Thành công thì lấy vị trí OldPos
                buf = User2System(virtAddr, charcount);
                // Trường hợp ghi file read & write
                if (fileSystem->openf[id]->type == 0)
                {
                    if ((fileSystem->openf[id]->Write(buf, charcount)) != 0)
                    {
                        // Số byte thực sự = NewPos - OldPos
                        NewPos = fileSystem->openf[id]->GetCurrentPos();
                        machine->WriteRegister(2, NewPos - OldPos);
                        delete[] buf;
                        increasePC();
                        return;
                    }
                }
                
            }
            case SC_Exec:
            {
                int name = machine->ReadRegister(4);
                if (name == 0) {
                    machine->WriteRegister(2, -1);
                    return;
                }
                char s[FileNameMaxLen+1] = {0};
                for (int j = 0; j < FileNameMaxLen; ++j) {
                    int oneChar = 0;
                    if (machine->ReadMem(name+j, 1, &oneChar) == FALSE) {
                        machine->WriteRegister(2, -1);
                        return;
                    }
                    if (oneChar == 0) break;
                    s[j] = (char)oneChar;
                }
                int ret = pTab->ExecUpdate(s);
                machine->WriteRegister(2, ret);
                increasePC();
                break;
            }
            case SC_Join:
            {
                int id = machine->ReadRegister(4);
                /*join process*/
                int ret = pTab->JoinUpdate(id);
                machine->WriteRegister(2, ret);
                increasePC();
                break;
            }
            case SC_Exit:
            {
                int exitCode = machine->ReadRegister(4);
                /*exit process*/
                int ret = pTab->ExitUpdate(exitCode);
                machine->WriteRegister(2, ret);
                increasePC();
                break;
            }
            case SC_CreateSemaphore:
            {
                int name = machine->ReadRegister(4);
                int semval = machine->ReadRegister(5);
                if (name == 0 || semval < 0) {
                    machine->WriteRegister(2, -1);
                    return;
                }
                char s[SEM_MAXNAMESIZE] = {0};
                for (int i = 0; i < SEM_MAXNAMESIZE-1; ++i) {
                    int oneChar = 0;
                    machine->ReadMem(name+i, 1, &oneChar);
                    if (oneChar == 0) break;
                    s[i] = (char)oneChar;
                }
                /*create semaphore*/
                int ret = semTab->Create(s, semval);
                machine->WriteRegister(2, ret);
                increasePC();
                break;
            }
            case SC_Wait:
            {
                int name = machine->ReadRegister(4);
                if (name == 0) {
                    machine->WriteRegister(2, -1);
                    return;
                }
                 
                char s[SEM_MAXNAMESIZE] = {0};
                for (int i = 0; i < SEM_MAXNAMESIZE-1; ++i) {
                    int oneChar = 0;
                    machine->ReadMem(name+i, 1, &oneChar);
                    if (oneChar == 0) break;
                    s[i] = (char)oneChar;
                }
                  
                int ret = semTab->Wait(s);
                machine->WriteRegister(2, ret);
                increasePC();
                break;
            }
            case SC_Signal:
            {
                int name = machine->ReadRegister(4);
                if (name == 0) {
                    machine->WriteRegister(2, -1);
                    return;
                }
                char s[SEM_MAXNAMESIZE] = {0};
                for (int i = 0; i < SEM_MAXNAMESIZE-1; ++i) {
                    int oneChar = 0;
                    machine->ReadMem(name+i, 1, &oneChar);
                    if (oneChar == 0) break;
                    s[i] = (char)oneChar;
                }
                /*signal semaphore*/
                int ret = semTab->Signal(s);
                machine->WriteRegister(2, ret);
                increasePC();
                break;
            }
        }
        break;
    }
}

