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
	string userMsg = "";
	msg.mtype = 113; // set mtype
	int qid = stoi(argv[0]); // qid from parent to access the same message queue
	
	cout << "Sender, PID " << getpid() << " begins execution" << endl;
	cout << "Enter a message to send: " << endl;
	cin >> userMsg;
	strcpy(msg.greeting, userMsg.c_str()); // strcpy(destination, source)
	
	/*
		Send message to message queue with id qid and mtype 113.
	*/
	msgsnd(qid, (struct msgbuf*)&msg, size, 0);
	
	cout << "Sender, PID " << getpid() << " sending message to receiver..." << endl;
	
	exit(0);
	
}
