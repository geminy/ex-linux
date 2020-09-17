#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int index;
	int msgid;

	for (index = 0; index < 10; ++index) {
		msgid = msgget(IPC_PRIVATE, IPC_CREAT);
		if (-1 == msgid) {
			perror("msgget error");
			exit(0);
		}
		printf("msgid = %d\n", msgid);
		
		msgctl(msgid, IPC_RMID, NULL);
	}

	exit(0);
}
