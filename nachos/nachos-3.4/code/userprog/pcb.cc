#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"

PCB::PCB(int id)
{
	joinsem= new Semaphore("JoinSem",0);
	exitsem= new Semaphore("ExitSem",0);
	mutex= new Semaphore("Mutex",1);
	pid= id;
	exitcode= 0;
	numwait= 0;
	if(id)
		parentID = currentThread->processID;
	else
		parentID= 0;
	thread= NULL;
	//JoinStatus= -1;

}

PCB::~PCB()
{
	if(joinsem != NULL)
		delete joinsem;
	if(exitsem != NULL)
		delete exitsem;
	if(mutex != NULL)
		delete mutex;
	if(thread != NULL)
	{
		thread->FreeSpace();
		thread->Finish();
	}
}

//------------------------------------------------------------------
int PCB::GetID()
{
	pid = thread->processID;
	return pid;
}

int PCB::GetNumWait()
{
	return numwait;
}

int PCB::GetExitCode()
{
	return exitcode;	
}

void PCB::SetExitCode(int ec)
{
	exitcode= ec;
}

void PCB::IncNumWait()
{
	numwait++;
}

void PCB::DecNumWait()
{
	if(numwait)
		numwait--;
}

char* PCB::GetNameThread()
{
	return thread->getName();
}

//-------------------------------------------------------------------
void PCB::JoinWait()
{
	JoinStatus= parentID;
	IncNumWait();
	joinsem->P();
}

void PCB::JoinRelease()
{
	DecNumWait();
	joinsem->V();
}

void PCB::ExitWait()
{
	exitsem->P();
}

void PCB::ExitRelease()
{
	exitsem->V();
}

//------------------------------------------------------------------
int PCB::Exec(char *filename, int pID)
{
	
	mutex->P();
	//printf("PCB ");
	
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
		mutex->V();
		return -1;
	}
	thread->processID= pID;
	parentID = currentThread->processID;
	thread->Fork(MyStartProcess,pID);
	
	mutex->V();
	//thread->Print();
	return pID;
}


//*************************************************************************************
void MyStartProcess(int pID)
{
	
	
	//printf("%d",pID);
	char *filename= processTab->GetName(pID);
	//printf("MyStartProc");
	//printf(filename);
	//if (0!=strcmp(filename,"./test/pong")){strcpy(filename,"./test/ping")	;}
	AddrSpace *space= new AddrSpace(filename);
	if(space == NULL)
	{
		printf("\nLoi: Khong du bo nho de cap phat cho tien trinh !!!\n");
		return; 
	}
	currentThread->space= space;
	//printf("MyStartProc: ");
	//currentThread->Print();
	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register

	machine->Run();			// jump to the user progam
	ASSERT(FALSE);			// machine->Run never returns;
						// the address space exits
						// by doing the syscall "exit"
}