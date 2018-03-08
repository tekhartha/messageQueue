// use_msgQ.cpp

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

void child_proc_one(int qid) {

	buf msg; // An instance of user-defined buffer.
	int size = sizeof(msg) - sizeof(long); // == the size of greeting[50] from buffer

	/*
		msgrcv removes a message of type 113 from the queue that qid represents
			but it must wait for a message to be sent (from process 1)
		Then, it puts that message into the buffer that msg points to.
		msgbuf is already defined in linux/msg.h
	*/
	msgrcv(qid, (struct msgbuf *)&msg, size, 113, 0);

	cout << getpid() << ": gets message" << endl; // getpid() returns pid of calling process
	cout << "message: " << msg.greeting << endl;

	strcat(msg.greeting, " and Adios."); // concatencate "and Adios" to end of msg
	cout << getpid() << ": sends reply" << endl;
	msg.mtype = 114; 	// prepare message with type mtype = 114
	msgsnd(qid, (struct msgbuf *)&msg, size, 0); // send a message with mtype 114
	cout << getpid() << ": now exits" << endl;
}

void child_proc_two(int qid) {

	buf msg;
	int size = sizeof(msg) - sizeof(long);

	// sending a message that will never be read
	msg.mtype = 12; // since there's no msgrcv that can take msgs with mtype 12, this msg is never received
	strcpy(msg.greeting, "Fake message");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	// prepare my message to send
	strcpy(msg.greeting, "Hello there");

	cout << getpid() << ": sends greeting" << endl;
	msg.mtype = 113; 	// set message type mtype = 113	
	msgsnd(qid, (struct msgbuf *)&msg, size, 0); // send a message with mtype 113

	msgrcv(qid, (struct msgbuf *)&msg, size, 114, 0); // receive a message of mtype 114 (from process 1)
	cout << getpid() << ": gets reply" << endl;
	cout << "reply: " << msg.greeting << endl;
	cout << getpid() << ": now exits" << endl;
}

int main() {

	/*
		msgget(key_t key, int flag)
		msgget returns the message queue identifier.
		IPC_PRIVATE as the key = new message queue.
		IPC_EXCL and IPC_CREAT in the flag make it so that an error 
			occurs if there is already a msg q with that key.
		0600 = permissions of message queue (read write execute for owner, group, others). octal
	*/
	int qid = msgget(IPC_PRIVATE, IPC_EXCL | IPC_CREAT | 0600);

	//spawning two child processes
	pid_t cpid = fork();
	if (cpid == 0) {
		child_proc_one(qid); // first process
		exit(0);
	}
	cpid = fork();
	if (cpid == 0) {
		child_proc_two(qid); // second process
		exit(0);
	}

	while (wait(NULL) != -1); // waiting for both children to terminate

	msgctl(qid, IPC_RMID, NULL); // remove the message queue associated with qid

	cout << "parent proc: " << getpid()
		<< " now exits" << endl;

	exit(0);
}