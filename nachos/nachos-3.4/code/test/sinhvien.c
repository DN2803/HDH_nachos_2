/*sinhvien program*/
#include "syscall.h"

int
main()
{
    OpenFileId sv, vn;
    SpaceId vnproc;
    char c[1];
    int ret;
    sv = Open("sinhvien.txt", 1);
    if (sv == -1) Exit(-1);

    vnproc = Exec("test/voinuoc");
    if (vnproc == -1) {
        Close(sv);
        Exit(-1);
    }

    ret = -1;

    while (ret != 0) {
        ret = CreateFile("voinuoc.txt");
        if (ret == -1) {
            Close(sv);
            Exit(-1);
        }

        vn = Open("voinuoc.txt", 0);
        if (vn == -1) {
            Close(sv);
            Exit(-1);
        }

        while (1) {
            if (Read(c, 1, sv) == 0) {
                Write("0", 1, vn);
                ret = 0;
                break;
            } else ret = -1;
            if (c[0] == ' ' || c[0] == '\n') break;
            Write(c, 1, vn);
        }

        Close(vn);
        Signal("voinuoc");
        Wait("sinhvien");
    }
    Exit(0);
}