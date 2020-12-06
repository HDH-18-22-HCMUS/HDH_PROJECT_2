#include "syscall.h"
#include "copyright.h"

int A[10];

int main()
{
    
	int a, i;
   PrintString("Nhap a: ");
   a = ReadInt();
   PrintString("Nhap mang: \n");
   
   for (i = 0; i < a; i++)
   {
      A[i] = ReadInt();
   }
   PrintString("\n");

   for (i = 0; i < a; i++)
   {
      PrintInt(A[i]);
      PrintChar(' ');
   }
   PrintString("\n");
    
	return 0;
}



















