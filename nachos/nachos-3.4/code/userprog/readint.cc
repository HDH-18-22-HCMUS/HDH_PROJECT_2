#include "synchcons.h"

int ReadInt(){
    int nByte = 10;
    char* into = new char[nByte];
    SynchConsole* sc = new SynchConsole();

    int len = sc->Read(into, nByte); // do dai ky tu chuoi nhap.

    if (len == -1) {
        return 0;
    }

    // Chuyen doi chuoi nhap thanh so.
    int result = 0; //Bien ket qua.
    int pow = 1;
    for (int i = 0; i < len; i++) {

        // Xet ton tai ky khong phai so. Tra ve 0 khi ton tai ky tu khong phai so.
        if ((into[i] < '0') || (into[i] > '9')) {
            return 0;
        }

        result = result + (into[i] - 48) * pow; // Them ky tu vao bien ket qua.
        pow = pow * 10;
    }
    return result;
}