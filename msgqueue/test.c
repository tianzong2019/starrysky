#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <pthread.h>

/* gcc msqu.c -lpthread */

#define MSG_FILE          "."
#define BUFFER            255
#define PERM              S_IRUSR|S_IWUSR
#define IPCKEY            0x111

struct msgtype {
	long mtype;
	char buffer[BUFFER+1];
};

void* thr_test( void* arg )
{
	struct msgtype msg;
	int msgid =  *((int*)arg);

	printf("msqid = %d  IPC_NOWAIT = %d\n", msgid, IPC_NOWAIT);
	time_t tt = time(0)+8;
	//while( time(0) <= tt )
	//{
	msgrcv(msgid, &msg, sizeof(struct msgtype), 1, 0);
	printf("Server Receive:%s\n", msg.buffer);
	msg.mtype = 2;
	msgsnd(msgid, &msg, sizeof(struct msgtype), 0);
	//}
	pthread_exit( (void*)2 );
}

int main(int argc, char **argv)
{
	struct msgtype msg;
	key_t key;
	int msgid;
	pthread_t tid;

	if(argc != 2) {
		fprintf(stderr,"Usage:%s string %d\n", argv[0], argc);
		exit(1);
	}
	/* key_t ftok(const char *pathname, int proj_id)
	把一个已存在的路径名和一个整数标识符转换成IPC键值
	
	   char path[256];
	   sprintf( path, "%s/", (char*)getenv("HOME") );
	   printf( "path is %s\n", path );
	   msgid=ftok( path, IPCKEY );
	   */
	if((key=ftok(MSG_FILE,'a'))==-1) {
		fprintf(stderr,"Creat Key Error:%s\n", strerror(errno));
		exit(1);
	}

	if((msgid=msgget(key, IPC_CREAT | 0666/*PERM*/))==-1) {
		fprintf(stderr,"Creat Message  Error:%s\n", strerror(errno));
		exit(1);
	}

	pthread_create( &tid, NULL, thr_test, &msgid );

	printf("msid is :%d\n", msgid);
	msg.mtype = 1;
	strncpy(msg.buffer, argv[1], BUFFER);
	msgsnd(msgid, &msg, sizeof(struct msgtype), 0);
	pthread_join(tid, NULL);

	return 0;
}
