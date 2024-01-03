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
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, counter);
	counter = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, counter);
	machine->WriteRegister(NextPCReg, counter + 4);
}

// Input: - User space address (int)
//- Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
	int i; //chi so index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //can cho chuoi terminal
	if (kernelBuf == NULL)
		return kernelBuf;

	memset(kernelBuf, 0, limit + 1);

	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
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
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);

	// Bien toan cuc cho lop SynchConsole

	//Bat dau
	switch (which)
	{
	case NoException:
		return;

	case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		interrupt->Halt();
		break;

	case ReadOnlyException:
		DEBUG('a', "\n Write attempted to page marked read-only");
		printf("\n\n Write attempted to page marked read-only");
		interrupt->Halt();
		break;

	case BusErrorException:
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		interrupt->Halt();
		break;

	case AddressErrorException:
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		interrupt->Halt();
		break;

	case OverflowException:
		DEBUG('a', "\nInteger overflow in add or sub.");
		printf("\n\n Integer overflow in add or sub.");
		interrupt->Halt();
		break;

	case IllegalInstrException:
		DEBUG('a', "\n Unimplemented or reserved instr.");
		printf("\n\n Unimplemented or reserved instr.");
		interrupt->Halt();
		break;

	case NumExceptionTypes:
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		interrupt->Halt();
		break;

	case SyscallException:
		switch (type)
		{

		case SC_Halt:
			// Input: Khong co
			// Output: Thong bao tat may
			// Chuc nang: Tat HDH
			DEBUG('a', "\nShutdown, initiated by user program. ");
			printf("\nShutdown, initiated by user program. ");
			interrupt->Halt();
			return;

		case SC_ReadInt:
		{
			int maxLength = 255;
			char *buf = new char[maxLength + 1];
			bool isNegative = false;
			int startIndex = 0;
			int ans = 0, size = 0, countMinus = 0;

			size = gSynchConsole->Read(buf, maxLength);

			if (size == 0)
			{
				DEBUG('a', "\n Invalid input number!");
				printf("\n\n Invalid input number!");

				machine->WriteRegister(2, 0);
				IncreasePC();
				delete buf;
				return;
			}

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
					IncreasePC();
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
					IncreasePC();
					delete buf;
					return;
				}
				else
					ans = ans * 10 + (buf[i] - '0');
			}

			if (isNegative)
				ans *= -1;

			machine->WriteRegister(2, ans);
			IncreasePC();
			delete buf;
			return;
		}

		case SC_PrintInt:
		{
			int number = machine->ReadRegister(4);

			if (number == 0)
			{
				gSynchConsole->Write("0", 1); // In ra man hinh so 0
				IncreasePC();
				return;
			}

			/*Qua trinh chuyen so thanh chuoi de in ra man hinh*/
			bool isNegative = false; // gia su la so duong
			int numberOfNum = 0;	 // Bien de luu so chu so cua number
			int firstNumIndex = 0;

			if (number < 0)
			{
				isNegative = true;
				number = number * -1; // Nham chuyen so am thanh so duong de tinh so chu so
				firstNumIndex = 1;
			}

			int t_number = number; // bien tam cho number
			while (t_number)
			{
				numberOfNum++;
				t_number /= 10;
			}

			// Tao buffer chuoi de in ra man hinh
			char *buffer;
			int MAX_BUFFER = 255;
			buffer = new char[MAX_BUFFER + 1];
			for (int i = firstNumIndex + numberOfNum - 1; i >= firstNumIndex; i--)
			{
				buffer[i] = (char)((number % 10) + 48);
				number /= 10;
			}
			if (isNegative)
			{
				buffer[0] = '-';
				buffer[numberOfNum + 1] = 0;
				gSynchConsole->Write(buffer, numberOfNum + 1);
				delete buffer;
				IncreasePC();
				return;
			}
			buffer[numberOfNum] = 0;
			gSynchConsole->Write(buffer, numberOfNum);
			delete buffer;
			IncreasePC();
			return;
		}

		case SC_ReadChar:
		{
			int maxBytes = 255;
			char *buffer = new char[255];
			int numBytes = gSynchConsole->Read(buffer, maxBytes);

			if (numBytes > 1) //Neu nhap nhieu hon 1 ky tu thi khong hop le
			{
				printf("Chi duoc nhap duy nhat 1 ky tu!");
				DEBUG('a', "\nERROR: Chi duoc nhap duy nhat 1 ky tu!");
				machine->WriteRegister(2, 0);
			}
			else if (numBytes == 0) //Ky tu rong
			{
				printf("Ky tu rong!");
				DEBUG('a', "\nERROR: Ky tu rong!");
				machine->WriteRegister(2, 0);
			}
			else
			{
				//Chuoi vua lay co dung 1 ky tu, lay ky tu o index = 0, return vao thanh ghi R2
				char c = buffer[0];
				machine->WriteRegister(2, c);
			}

			delete buffer;
			break;
		}

		case SC_PrintChar:
		{
			char c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
			gSynchConsole->Write(&c, 1);			 // In ky tu tu bien c, 1 byte
			break;
		}

		case SC_ReadString:
		{
			int virtAddr, length;
			char *buffer;
			virtAddr = machine->ReadRegister(4);	// Lay dia chi tham so buffer truyen vao tu thanh ghi so 4
			length = machine->ReadRegister(5);		// Lay do dai toi da cua chuoi nhap vao tu thanh ghi so 5
			buffer = User2System(virtAddr, length); // Copy chuoi tu vung nho User Space sang System Space
			gSynchConsole->Read(buffer, length);	// Goi ham Read cua SynchConsole de doc chuoi
			System2User(virtAddr, length, buffer);	// Copy chuoi tu vung nho System Space sang vung nho User Space
			delete buffer;
			IncreasePC(); 
			return;
		}

		case SC_PrintString:
		{
			int virtAddr;
			char *buffer;
			virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
			buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu
			int length = 0;
			while (buffer[length] != 0)
				length++;							  // Dem do dai that cua chuoi
			gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
			delete buffer;
			break;
		}

		case SC_CreateFile:
		{
			int virtAddr;
			char *filename;
			DEBUG('a', "\n SC_CreateFile call ...");
			DEBUG('a', "\n Reading virtual address of filename");

			virtAddr = machine->ReadRegister(4); //Doc dia chi cua file tu thanh ghi R4
			DEBUG('a', "\n Reading filename.");

			//Sao chep khong gian bo nho User sang System, voi do dang toi da la (32 + 1) bytes
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (strlen(filename) == 0)
			{
				printf("\n File name is not valid");
				DEBUG('a', "\n File name is not valid");
				machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				break;
			}

			if (filename == NULL) //Neu khong doc duoc
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				delete filename;
				break;
			}
			DEBUG('a', "\n Finish reading filename.");

			if (!fileSystem->Create(filename, 0)) //Tao file bang ham Create cua fileSystem, tra ve ket qua
			{
				//Tao file that bai
				printf("\n Error create file '%s'", filename);
				machine->WriteRegister(2, -1);
				delete filename;
				break;
			}

			//Tao file thanh cong
			machine->WriteRegister(2, 0);
			delete filename;
			break;
		}

		case SC_Open:
		{
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
			int type = machine->ReadRegister(5);	 // Lay tham so type tu thanh ghi so 5
			char *filename;
			filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
			//Kiem tra xem OS con mo dc file khong

			int freeSlot = fileSystem->FindFreeSlot();
			if (freeSlot != -1) //Chi xu li khi con slot trong
			{
				if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
				{

					if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
					{
						machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
					}
				}
				else if (type == 2) // xu li stdin voi type quy uoc la 2
				{
					machine->WriteRegister(2, 0); //tra ve OpenFileID
				}
				else // xu li stdout voi type quy uoc la 3
				{
					machine->WriteRegister(2, 1); //tra ve OpenFileID
				}
				delete[] filename;
				break;
			}
			machine->WriteRegister(2, -1); //Khong mo duoc file return -1

			delete[] filename;
			break;
		}

		case SC_Close:
		{
			int fid = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
			if (fid >= 0 && fid <= 14)			//Chi xu li khi fid nam trong [0, 14]
			{
				if (fileSystem->openf[fid]) //neu mo file thanh cong
				{
					delete fileSystem->openf[fid]; //Xoa vung nho luu tru file
					fileSystem->openf[fid] = NULL; //Gan vung nho NULL
					machine->WriteRegister(2, 0);
					break;
				}
			}
			machine->WriteRegister(2, -1);
			break;
		}

		case SC_Read:
		{
			int virtAddr = machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = machine->ReadRegister(6);		  // Lay id cua file tu thanh ghi so 6
			int OldPos;
			int NewPos;
			char *buf;
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the read vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the read vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if (fileSystem->openf[id]->type == 3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra ve -1
			{
				printf("\nKhong the read file stdout.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
			buf = User2System(virtAddr, charcount);			 // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			// Xet truong hop doc file stdin (type quy uoc la 2)
			if (fileSystem->openf[id]->type == 2)
			{
				// Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
				int size = gSynchConsole->Read(buf, charcount);
				System2User(virtAddr, size, buf); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
				machine->WriteRegister(2, size);  // Tra ve so byte thuc su doc duoc
				delete buf;
				IncreasePC();
				return;
			}
			// Xet truong hop doc file binh thuong thi tra ve so byte thuc su
			if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
			{
				// So byte thuc su = NewPos - OldPos
				NewPos = fileSystem->openf[id]->GetCurrentPos();
				// Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
				System2User(virtAddr, NewPos - OldPos, buf);
				machine->WriteRegister(2, NewPos - OldPos);
			}
			else
			{
				// Truong hop con lai la doc file co noi dung la NULL tra ve -2
				//printf("\nDoc file rong.");
				machine->WriteRegister(2, -2);
			}
			delete buf;
			IncreasePC();
			return;
		}

		case SC_Write:
		{
			int virtAddr = machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = machine->ReadRegister(6);		  // Lay id cua file tu thanh ghi so 6
			int OldPos;
			int NewPos;
			char *buf;
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the write vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the write vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
			if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
			{
				printf("\nKhong the write file stdin hoac file only read.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
			buf = User2System(virtAddr, charcount);			 // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			// Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
			if (fileSystem->openf[id]->type == 0)
			{
				if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
				{
					// So byte thuc su = NewPos - OldPos
					NewPos = fileSystem->openf[id]->GetCurrentPos();
					machine->WriteRegister(2, NewPos - OldPos);
					delete buf;
					IncreasePC();
					return;
				}
			}
			if (fileSystem->openf[id]->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
			{
				int i = 0;
				while (buf[i] != 0 && buf[i] != '\n') // Vong lap de write den khi gap ky tu '\n'
				{
					gSynchConsole->Write(buf + i, 1); // Su dung ham Write cua lop SynchConsole
					i++;
				}
				buf[i] = '\n';
				gSynchConsole->Write(buf + i, 1); // Write ky tu '\n'
				machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
				delete buf;
				IncreasePC();
				return;
			}
		}

		case SC_Seek:
		{
			int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
			int id = machine->ReadRegister(5);	// Lay id cua file
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the seek vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra co goi Seek tren console khong
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
			if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
			{
				printf("\nKhong the seek file den vi tri nay.");
				machine->WriteRegister(2, -1);
			}
			else
			{
				// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
				fileSystem->openf[id]->Seek(pos);
				machine->WriteRegister(2, pos);
			}
			IncreasePC();
			return;
		}
		case SC_Exec:
		{
			int virtAddr;
			virtAddr = machine->ReadRegister(4); // doc dia chi ten chuong trinh tu thanh ghi r4
			char *name;
			name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel

			if (name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				machine->WriteRegister(2, -1);
				//IncreasePC();
				return;
			}
			OpenFile *oFile = fileSystem->Open(name);
			if (oFile == NULL)
			{
				printf("\nExec:: Can't open this file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

			delete oFile;

			// Return child process id
			int id = pTab->ExecUpdate(name);
			machine->WriteRegister(2, id);

			delete[] name;
			IncreasePC();
			return;
		}
		case SC_Join:
		{
			int id = machine->ReadRegister(4);

			int res = pTab->JoinUpdate(id);

			machine->WriteRegister(2, res);
			IncreasePC();
			return;
		}
		case SC_Exit:
		{
			int exitStatus = machine->ReadRegister(4);

			if (exitStatus != 0)
			{
				IncreasePC();
				return;
			}

			int res = pTab->ExitUpdate(exitStatus);

			currentThread->FreeSpace();
			currentThread->Finish();
			IncreasePC();
			return;
		}
		case SC_CreateSemaphore:
		{
			int virtAddr = machine->ReadRegister(4);
			int semval = machine->ReadRegister(5);

			char *name = User2System(virtAddr, MaxFileLength + 1);
			if (name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}

			int res = semTab->Create(name, semval);

			if (res == -1)
			{
				DEBUG('a', "\n Khong the khoi tao semaphore");
				printf("\n Khong the khoi tao semaphore");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}

			delete[] name;
			machine->WriteRegister(2, res);
			IncreasePC();
			return;
		}

		case SC_Wait:
		{
			int virtAddr = machine->ReadRegister(4);

			char *name = User2System(virtAddr, MaxFileLength + 1);
			if (name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}

			int res = semTab->Wait(name);

			if (res == -1)
			{
				DEBUG('a', "\n Khong ton tai ten semaphore nay!");
				printf("\n Khong ton tai ten semaphore nay!");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}

			delete[] name;
			machine->WriteRegister(2, res);
			IncreasePC();
			return;
		}
		case SC_Signal:
		{
			int virtAddr = machine->ReadRegister(4);

			char *name = User2System(virtAddr, MaxFileLength + 1);
			if (name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}

			int res = semTab->Signal(name);

			if (res == -1)
			{
				DEBUG('a', "\n Khong ton tai ten semaphore nay!");
				printf("\n Khong ton tai ten semaphore nay!");
				machine->WriteRegister(2, -1);
				delete[] name;
				IncreasePC();
				return;
			}

			delete[] name;
			machine->WriteRegister(2, res);
			IncreasePC();
			return;
		}
		default:
			break;
		}
		IncreasePC();
	}
}
