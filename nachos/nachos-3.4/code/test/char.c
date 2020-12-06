#include "syscall.h"
#include "copyright.h"
int main()
{
	char c = ReadChar();
	//char c= 'e';
	PrintChar(c);
	return 0;
}