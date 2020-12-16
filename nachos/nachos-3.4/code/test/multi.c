#include "syscall.h"
void main()
{
    int pingPID, pongPID;
    PrintString("Ping-Pong test starting ...\n\n");
    //pingPID = Exec("./test/ping");
    pongPID = Exec("./test/pong");
    pingPID = Exec("./test/ping");
    Join(pingPID);
    Join(pongPID);
    
}
