// Name : Maximillian Risdall, Kun Ki Lee, Julia Brooks
// Project 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGSZ 4096
	typedef struct msgbuf
	{
	long mtype;
	char mtext[MSGSZ];
	}mymsg_t;

int main(int argc, char* argv[]) {
	if(argc != 1)
	{
		printf("No commands expect and %d given! \n", argc - 1);
		exit(1);
	}

	int server_direct_in = shmget(222, MSGSZ * sizeof(char), IPC_CREAT | 0666);
	if(server_direct_in < 0)
	{
		printf("shmget error 1\n");
		exit(1);
	}

	int * server_direct_in_memory = (int *) shmat(server_direct_in, NULL, 0);//attach to shared memory for client2
	while(1)
	{
		mymsg_t msg;
		int status = msgget(server_direct_in_memory[0], 0666| IPC_CREAT);
		if(status == -1)
		{
			perror("ERROR MESSAGE");
		}
		if(msgrcv(status, (void *)&msg, sizeof(msg), 0, 0) < 0){
			perror("ERROR");
		} //allows us to take in any type of messages from message queues

		if(msg.mtype == 2)//when message type is a 1 to 1 message
		{
			printf("Message Recieved : %s", msg.mtext);
			printf("Write Response : ");
			mymsg_t rmsg;
			fgets(rmsg.mtext, 4096, stdin); //grab user input for response
			rmsg.mtext[strlen(rmsg.mtext)] = '\0';
			rmsg.mtype = 4;
			if(msgsnd(status, (void *)&rmsg, strlen(rmsg.mtext)+1,0) < 0)
			{
				perror("ERROR");
			}//send user's response
		}
		else if(msg.mtype == 3)//when message type is a broadcast
		{
			printf("Message Recieved : %s", msg.mtext);
		}

		else if(msg.mtype == 5)//when message type is exit
		{
			printf("%s", msg.mtext);
			exit(1);
		}

	}
}
