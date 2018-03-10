#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
using namespace std;

// declare my global message buffer. Template for message data
struct buf {
	long mtype; // required
	char greeting[50]; // message content, can hold any type of data
};

int main(int argc, char **argv) // parameters in main to direct program
{	
	buf msg; // declare buffer
	int size = sizeof(msg) - sizeof(long); // size of greeting
	int qid = stoi(argv[0]); // qid from parent to access the same message queue
	cout << "Receiver, PID " << getpid() << " begins execution" << endl;
	msgrcv(qid, (struct msgbuf*)&msg, size, 113, 0); // receive msg of mtype 113
	cout << "Receiver, PID " << getpid() << " has received message from sender..." << endl;
	cout << "Message: ***" << msg.greeting << "***" << endl;
	
	exit(0);
}
