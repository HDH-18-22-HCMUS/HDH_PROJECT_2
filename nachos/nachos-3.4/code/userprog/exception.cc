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
#define MAX_LENGTH 255
// To not show error
//#include "synchcons.h"
//SynchConsole *gSynchConsole=new SynchConsole();

#include "machine.h"
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
// And don't forget to incremenSt the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------
void ThreadFuncForUserProg(int arg)
{
    switch (arg)
    {
    case 0: // Fork
        // Fork just restore registers.
        currentThread->RestoreUserState();
        break;
    case 1: // Exec
        if (currentThread->space != NULL)
        {
            // Exec should initialize registers and restore address space.
            currentThread->space->InitRegisters();
            currentThread->space->RestoreState();
        }
        break;
    default:
        break;
    }

    machine->Run();
}
void AdvanceProgramCounter()
{
    // Advance program counters.
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    //machine->registers[PrevPCReg] = machine->registers[PCReg];
    // for debugging, in case we
    // are jumping into lala-land
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    //machine->registers[PCReg] = machine->registers[NextPCReg];
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
    //machine->registers[NextPCReg] += 4;
}

/*
Copy buffer from userspace to kernelspace

Arguments:
int virtualAddress: Userspace address
int bufferSize: maximum size of buffer

Output:
buffer
*/
char *User2System(int virtualAddress, int bufferSize)
{
    //Create kernel buffer to hold memory contents
    int chr;
    char *kernelBuffer = NULL;
    kernelBuffer = new char[bufferSize + 1];

    //Check if buffer was allocated successfully
    if (kernelBuffer == NULL)
    {
        return kernelBuffer;
    }

    //Clear buffer contents
    memset(kernelBuffer, 0, bufferSize + 1);

    //Copy memory from userspace to kernelspace, one byte at a time
    for (int i = 0; i < bufferSize; i++)
    {
        machine->ReadMem(virtualAddress + i, 1, &chr);
        kernelBuffer[i] = (char)chr;
        if (chr == 0)
            break;
    }
    return kernelBuffer;
}

/*
Copy buffer from kernelspace to userspace

Arguments:
int virtualAddress: Userspace address
int bufferSize: maximum size of buffer
char* buffer: pointer to userspace buffer

Output:
int counter: number of bytes copied
*/

int System2User(int virtualAddress, int bufferSize, char *buffer)
{
    //Check buffer validity
    if (bufferSize < 0)
    {
        return -1;
    }
    //If buffer is empty
    if (bufferSize == 0)
    {
        return 0;
    }

    int counter = 0;
    int chr = 0;

    //Copy memory from kernelspace to userspace, one byte at a time
    do
    {
        chr = (int)buffer[counter];
        machine->WriteMem(virtualAddress + counter, 1, chr);
        counter++;
    } while (counter < bufferSize && chr != 0);

    return counter;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch (which)
    {
    case SyscallException:
    {
        SynchConsole *gSynchConsole = new SynchConsole();
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
            break;
        }
        case SC_Sub:
        {
            int op1 = machine->ReadRegister(4);
            int op2 = machine->ReadRegister(5);
            int result = op1 - op2;
            machine->WriteRegister(2, result);
            //DEBUG('a', "123dfhh.\n");
            break;
        }

        case SC_ReadChar:
        {
            int sz = 0;
            char *buf = new char[MAX_LENGTH];
            sz = gSynchConsole->Read(buf, MAX_LENGTH);
            machine->WriteRegister(2, buf[0]);
            delete[] buf;
            break;
        }

        case SC_PrintChar:
        {
            char ch = (char)machine->ReadRegister(4);
            gSynchConsole->Write(&ch, 1);
            break;
        }

        case SC_ReadInt:
        {
            char ch;
            int result = 0; // Ket qua tra ve
            int len = 0;
            bool isNeg = false; // xac nhan kha nang chuoi la so am
            bool isNum = true;  // xac nhan chuoi la so nguyen
            bool isOf = false;  // xac nhan tinh trang Overflow

            while (true)
            {
                gSynchConsole->Read(&ch, 1);

                //Xet ky tu dau
                if (len == 0)
                {
                    //Truong hop khong xet den
                    if ((ch == ' ') || (ch == '\0'))
                    {
                        continue;
                    }

                    // Truong hop kha nang nhap so am
                    if (ch == '-')
                    {
                        isNeg = true;
                        len++;
                        continue;
                    }

                    // Truong hop kha nang nhap so duong
                    if (ch == '+')
                    {
                        len++;
                        continue;
                    }
                }

                //Xet truong hop dung vong lap
                if ((ch == ' ') || (ch == '\0'))
                {
                    break;
                }

                if (isNum)
                {
                    //Xac nhan ky tu la chu so hay khong
                    if ((ch < '0') || (ch > '9'))
                    {
                        isNum = false;
                        result = 0;
                    }
                    else
                    {
                        if (!isOf)
                        {
                            int n = (int)(ch & 0xf); //Chuyen doi chu so thanh so
                            int temp = result * 10 + n;
                            // Xet truong hop bat thuong de nhan dinh Overflow
                            if ((temp <= result) && (result != 0))
                            {
                                isOf = true;
                            }
                            else
                            {
                                result = temp;
                            }
                        }
                    }
                }
                len++;
            }

            // Dinh dang lai ket qua tra ve
            if (isNum)
            {
                if (isOf)
                {
                    //printf("Overflow\n"); //Debug Overflow
                    if (isNeg)
                    {
                        result = 0x80000000; // So am nhor nhat co the bieu dien
                    }
                    else
                    {
                        result = 0x7FFFFFFF; //So nguyen duong lon nhat co the bieu dien.
                    }
                }
                else
                {
                    if (isNeg)
                    {
                        result = -result;
                    }
                }
            }
            else
            {
                result = 0;
            }

            machine->WriteRegister(2, result);

            break;
        }

        case SC_PrintInt:
        {
            int num = machine->ReadRegister(4);
            char *str = new char[11]; // Dãy chữ số của num
            int nByte = 0;            // Số chữ số trong num.
            char neg = '-';

            if (num < 0)
            {
                gSynchConsole->Write(&neg, 1);
                num = -num;
            }

            int q = num;

            // Chuyển đổi từng chữ số trong num sang kiểu char
            // Dữ liệu bắt đầu lưu từ str[0] đến str[nByte] (từ trái sang phải)
            // Dừng vòng lặp khi str đã lưu hết các chữ số của num

            while (q >= 10)
            {
                q = q / 10;
                nByte++;
            }

            int count = nByte;
            nByte = nByte + 1;
            int j, r, temp;
            int i = 0;

            for (; i < nByte - 1; i++)
            {
                r = 1;
                for (j = 0; j < count; j++)
                {
                    r = r * 10;
                }
                temp = num / r;
                str[i] = temp + 48;
                gSynchConsole->Write(str + i, 1);
                num = num - (temp * r);
                count--;
            }
            str[nByte] = num + 48;
            gSynchConsole->Write(str + nByte, 1);

            delete[] str;
            break;
        }

        case SC_PrintString:
        {
            int i = 0;
            char *string = new char[MAX_LENGTH];

            //Copy string contents from user memory to kernel memory
            string = User2System(machine->ReadRegister(4), MAX_LENGTH + 1);

            //Print the first to second to last character of the string, one at a time
            while (string[i] != 0 && string[i] != '\n')
            {
                gSynchConsole->Write(string + i, 1);
                i++;
            }
            //Print the last character of the string
            gSynchConsole->Write(string + i, 1);

            //Free up unused memory
            delete[] string;
            break;
        }

        case SC_ReadString:
        {
            //Create string buffer
            char *string = new char[MAX_LENGTH];

            //Check string buffer validity
            if (string == 0) // out of save space
            {
                delete[] string;
                break;
            }

            int virtualAddress = machine->ReadRegister(4);
            int length = machine->ReadRegister(5);

            //Read string from console
            int bufferSize = gSynchConsole->Read(string, length);

            //Copy string contents from kernel memory to user memory
            System2User(virtualAddress, bufferSize, string);

            //Free up memory
            delete[] string;
            break;
        }

        case SC_Exec:
        {
            // Input: vi tri int
            // Output: Fail return -1, Success: return id cua thread dang chay
            // SpaceId Exec(char *name);
            int virtAddr;
            virtAddr = machine->ReadRegister(4); // doc dia chi ten chuong trinh tu thanh ghi r4
            char *name;
            name = User2System(virtAddr, MAX_LENGTH); // Lay ten chuong trinh, nap vao kernel
            if (name == NULL)
            {
                DEBUG('a', "\n Not enough memory in System");
                printf("\n Not enough memory in System");
                machine->WriteRegister(2, -1);
                break;
            }
            OpenFile *oFile = fileSystem->Open(name);
            if (oFile == NULL)
            {
                printf("\nExec:: Can't open this file.");
                machine->WriteRegister(2, -1);
                break;
            }

            delete oFile;

            // Return child process id
            int id = processTab->ExecUpdate(name);
            machine->WriteRegister(2, id);

            delete[] name;
            break;
        }

        case SC_Join:
        {
            // int Join(SpaceId id)
            // Input: id dia chi cua thread
            // Output:
            int id = machine->ReadRegister(4);

            int res = processTab->JoinUpdate(id);

            machine->WriteRegister(2, res);
            break;
        }
/*
        case SC_Exit:
        {
            //void Exit(int status);
            // Input: status code
            int exitStatus = machine->ReadRegister(4);

            if (exitStatus != 0)
            {
                break;
            }

            int res = processTab->ExitUpdate(exitStatus);
            //machine->WriteRegister(2, res);

            currentThread->FreeSpace();
            currentThread->Finish();
            break;
        }
*/
        default:
        {
            printf("Unexpected user mode exception %d %d\n", which, type);
            interrupt->Halt();
            break;
        }
        }
        delete gSynchConsole;
        AdvanceProgramCounter();
        break;
    }
    case NoException:
    {
        return;
    }
    case PageFaultException:
    {
        DEBUG('a', "No valid translation found.\n");
        printf("No valid translation found");
        interrupt->Halt();
        break;
    }
    case ReadOnlyException:
    {
        DEBUG('a', "Write attempted to page marked read-only.\n");
        printf("Write attempted to page marked read-only");
        interrupt->Halt();
        break;
    }
    case BusErrorException:
    {
        DEBUG('a', "Translation resulted in an invalid physical address.\n");
        printf("Translation resulted in an invalid physical address");
        interrupt->Halt();
        break;
    }

    case AddressErrorException:
    {
        DEBUG('a', "Unaligned reference or one that was beyond the end of the address space.\n");
        printf("Unaligned reference or one that was beyond the end of the address space");
        interrupt->Halt();
        break;
    }
    case OverflowException:
    {
        DEBUG('a', "Integer overflow in add or sub.\n");
        printf("Integer overflow in add or sub");
        interrupt->Halt();
        break;
    }
    case IllegalInstrException:
    {
        DEBUG('a', "Unimplemented or reserved instr..\n");
        printf("Unimplemented or reserved instr.");
        interrupt->Halt();
        break;
    }
    case NumExceptionTypes:
    {
        DEBUG('a', "Number exception types.\n");
        printf("Number exception types");
        interrupt->Halt();
        break;
    }
    }
}
