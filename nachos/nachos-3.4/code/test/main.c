/*main program*/
#include "syscall.h"

int main()
{
    OpenFileId inputID, outID, svID, resID;
    char c[1];
    int n, ret, count;
    SpaceId svproc;

    ret = CreateFile("output.txt");
    if (ret == -1) Exit(-1);

    ret = CreateSemaphore("sinhvien", 0);
    if (ret == -1) Exit(-1);

    ret = CreateSemaphore("voinuoc", 0);
    if (ret == -1) Exit(-1);

    inputID = Open("input.txt", 1);
    if (inputID == -1) Exit(-1);

    outID = Open("output.txt", 0);
    if (outID == -1) {
        Close(inputID);
        Exit(-1);
    }

    n = 0;

    // read first line
    while (1) { 
        if (Read(c, 1, inputID) < 1) {
            break;
        }

        if (c[0] >= '0' && c[0] <= '9'){
            n = n*10 + c[0] - '0';
        }
        
        if (c[0] == '\n') break;
    }

    PrintString("\t Number of times: ");
    PrintInt(n);
    PrintChar('\n');

    while (n--) {
        ret = CreateFile("sinhvien.txt");
        if (ret == -1) {
            Close(inputID);
            Exit(-1);
        }

        svID = Open("sinhvien.txt", 0);
        if (svID == -1) {
            Close(inputID);
            Exit(-1);
        }


        while (1) {
            if (Read(c, 1, inputID) < 1) {
                break;
            }
            count++;

            if (Write(c, 1, svID) < 1) {
                PrintString("Write sinhvien.txt error!\n");
            }
            
            if (c[0] == '\n') break;
        }        

        Close(svID);
        svproc = Exec("test/sinhvien");

        if (svproc != -1) {
            ret = Join(svproc);
        }
        
        Wait("sinhvien");
        Wait("voinuoc");

        if (ret == -1) {
            Close(inputID);
            Close(outID);
            Exit(-1);
        }

        // Write output.txt

        resID = Open("result.txt", 0);
        if (resID == -1) {
            Close(inputID);
            Close(outID);
            Exit(-1);
        }

        while (Read(c, 1, resID) != 0) {
            Write(c, 1, outID);
        }

        Close(resID);
    }

    Exit(0);
}

