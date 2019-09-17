// Name : Maximillian Risdall, Kun Ki Lee, Julia Brooks
// Project 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct msgbuf {
	long mtype;
	char mtext[4096];
} mymsg_t;

int main(int argc, char* argv[]) {
	if(argc != 1) {
		printf("No commands expect and %d given! \n", argc - 1);
		exit(1);
	}
	int common = shmget(111, 4096 * sizeof(char), IPC_CREAT | 0666);
	if(common < 0) {
		printf("shmget error 2\n");
		exit(1);
	}

	int * uncommon_key_memory = (int *) shmat(common, NULL, 0);
	uncommon_key_memory[0] = ftok(".", 'a');

	int msg_queue = msgget(uncommon_key_memory[0], IPC_CREAT | 0666);

	if(msg_queue == -1) {
		printf("Failed to create message queue!\n");
		exit(1);
	}

	int common2 = shmget(222, 4096 * sizeof(char), IPC_CREAT | 0666);
	if(common2 < 0) {
		printf("shmget error 3\n");
		exit(1);
	}

	int * uncommon_key_memory2 = (int *) shmat(common2, NULL, 0);
	uncommon_key_memory2[0] = ftok("..", 'b');

	int msg_queue2 = msgget(uncommon_key_memory2[0], IPC_CREAT | 0666);

	if(msg_queue2 == -1) {
		printf("Failed to create message queue!\n");
		exit(1);
	}

	while(1) {
		printf("Enter 1 to choose client 1\n      2 to choose client 2\n      3 for broadcasting\n      0 to exit.\n\nEnter choice: ");

		char input[16];
		fgets(input, 16, stdin);
		input[strlen(input) - 1] = '\0';

		if(strcmp(input, "0") == 0) {
			//take care of deleting applied memory
			mymsg_t msg5 = {5, "Server closed connection.\n"};
			if(msgsnd(msg_queue, (void *) &msg5, sizeof(msg5), 0) < 0)
			{
				perror("ERROR");
			}
			if(msgctl(msg_queue, IPC_RMID, 0) < 0)
			{
				perror("ERROR");
			}
			if(shmctl(common, IPC_RMID, 0) < 0)
			{
				perror("ERROR");
			}
			shmdt((void *) uncommon_key_memory);

			mymsg_t msg51 = {5, "Server closed connection.\n"};
			if(msgsnd(msg_queue2, (void *) &msg51, sizeof(msg51), 0) < 0)
			{
				perror("ERROR");
			}
			if(msgctl(msg_queue2, IPC_RMID, 0) < 0)
			{
				perror("ERROR");
			}
			if(shmctl(common2, IPC_RMID, 0) < 0)
			{
				perror("ERROR");
			}
			if(shmdt((void *) uncommon_key_memory2) < 0)
			{
				perror("ERROR");
			}

			exit(0);
		}

		else if(strcmp(input, "1") == 0) {
			mymsg_t msg1;
			msg1.mtype = 1;
			printf("Write Message : ");
			fgets(msg1.mtext, 4096, stdin);
			//send to client 1
			if(msgsnd(msg_queue, (void *) &msg1, sizeof(msg1), 0) < 0)
			{
				perror("ERROR");
			}

			//receive from client 1
			if(msgrcv(msg_queue, (void *) &msg1, sizeof(msg1), 4, 0) < 0)
			{
				perror("ERROR");
			}
			printf("Message Received : %s\n", msg1.mtext);
		}

		else if(strcmp(input, "2") == 0) {
			mymsg_t msg2;
			msg2.mtype = 2;
			printf("Write Message : ");
			fgets(msg2.mtext, 4096, stdin);
			//send and receive message from client 2
			if(msgsnd(msg_queue2, (void *) &msg2, sizeof(msg2), 0) < 0)
			{
				perror("ERROR");
			}
			//receive from server
			if(msgrcv(msg_queue2, (void *) &msg2, sizeof(msg2), 4, 0) < 0)
			{
				perror("ERROR");
			}
			printf("Message Received : %s\n", msg2.mtext);
		}

		else if(strcmp(input, "3") == 0) {
			mymsg_t msg3;
			msg3.mtype = 3;
			printf("Write Message : ");
			fgets(msg3.mtext, 4096, stdin);
			//send message to clients 1 and 2 via broadcast channel
			if(msgsnd(msg_queue, (void *) &msg3, sizeof(msg3), 0) < 0)
			{
				perror("ERROR");
			}
			if(msgsnd(msg_queue2, (void *) &msg3, sizeof(msg3), 0) < 0)
			{
				perror("ERROR");
			}
		}

		else {
			printf("Invalid input. Enter 1 or 2.\n\n");
		}
	}
	return 0;
}
