/*voinuoc program*/
#include "syscall.h"

int main()
{
    int n, v1, v2;
    OpenFileId voinuocID, resID;
    char c[1];
    CreateFile("result.txt");
    resID = Open("result.txt", 0);
    n = -1;
    v1 = v2 = 0;

    while (1) {
        if (n == 0) break;
        Wait("voinuoc");
        voinuocID = Open("voinuoc.txt", 1);
        if (voinuocID == -1) Exit(-1);
        
        n = 0;

        while (1) {
            if (Read(c, 1, voinuocID) == 0) break;
            if (c[0] >= '0' && c[0] <= '9') n = 10*n + c[0] - '0';
            else break;
        }

        Close(voinuocID);

        if (n != 0) {
            if (v1 <= v2) {
                v1 += n;

                if (resID != -1) 
                    Write("1 ", 2, resID);
            } else {
                v2 += n;
                
                if (resID != -1)
                    Write("2 ", 2, resID);
            }
        }

        Signal("sinhvien");
    }

    if (resID != -1) {
        Write("\n", 1, resID);
        Close(resID);
    }

    Exit(0);
}
