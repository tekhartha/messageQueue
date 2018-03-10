#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
using namespace std;

int main()
{
	/*
		Message queue identifier.
		msgget(key_t key, int flag)
		IPC_PRIVATE = new message queue 
		IPC_EXCL, CREAT = error if existing message queue with same key
		0600 = permissions
	*/
	
	int qid = msgget(IPC_PRIVATE, IPC_EXCL | IPC_CREAT | 0600);
	string qid_str = to_string(qid);
	static char *arg[] = {(char*)qid_str.c_str(), NULL}; // pass this array into child procs
	
	cout << "Message queue ID: " << qid << endl; // Print message queue ID
	
	/*
		First child process created: sender
	*/
	
	pid_t child_pid = fork();
	if (child_pid == 0)
	{
		cout << "Sender PID: " << getpid() << endl; 
		execv("sender", arg);
		// if execv returns, it has failed
		exit(0);
	}
	
	/*
		Second child process created: receiver
	*/

	child_pid = fork();
	if (child_pid == 0)
	{
		cout << "Receiver PID: " << getpid() << endl; 
		execv("receiver", arg);
		// if execv returns, it has failed
		exit(0);
	}
	
	while(wait(NULL) != -1); // wait for both children to terminate
	
	msgctl(qid, IPC_RMID, NULL); // remove message queue
	
	cout << "Parent proc: " << getpid() << " now exits." << endl;
	
	exit(0);
	
}
