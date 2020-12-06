#include "syscall.h"
#include "copyright.h"

#define M 100

int main()
{

    int A[M];
    int n, i, j, temp, choice;

    do
    {
        PrintString("Nhap n (n <= 100): ");
        n = ReadInt();
    } while (n<1 || n>100);
    // nhap n

    // nhap mang
    PrintString("Nhap cac phan tu mang A: \n");
    for (i = 0; i < n; i++)
    {
        A[i] = ReadInt();
    }
    PrintString("\n");

    PrintString("1 - Sap xep tang dan \n");
    PrintString("2 - Sap xep giam dan \n");

    do
    {
        PrintString("=> Nhap lua chon: ");
        choice = ReadInt();
    } while (choice != 1 && choice != 2);

    if (choice == 1)
    {
        // sap xep mang tang dan
        for (i = 0; i < n - 1; i++)
        {
            for (j = i + 1; j < n; j++)
            {
                if (A[j] < A[i])
                {
                    temp = A[j];
                    A[j] = A[i];
                    A[i] = temp;
                }
            }
        }
    }

    else if (choice == 2)
    {
        // sap xep mang giam dan
        for (i = 0; i < n - 1; i++)
        {
            for (j = i + 1; j < n; j++)
            {
                if (A[j] > A[i])
                {
                    temp = A[j];
                    A[j] = A[i];
                    A[i] = temp;
                }
            }
        }
    }

    PrintString("\n");
    // xuat mang sau khi sap xep
    PrintString("Mang A (da sap xep): ");

    for (i = 0; i < n; i++)
    {
        PrintInt(A[i]);
        PrintChar(' ');
    }
    PrintString("\n");

    return 0;
}