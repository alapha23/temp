#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_sched_setweight      380
#define __NR_sched_getweight      381


int main(int args, char **argv)
{

        int flag;
	int weight;
	int pid;
	int status;

	if((pid=fork())!=0)
	{
		weight = 11;
	        flag = syscall(__NR_sched_setweight, pid, weight);
	        printf("SET: Process %d weight returned: %d \n", pid, flag);
	
		flag = syscall(__NR_sched_getweight, pid, weight);
	        printf("GET: Process %d has weight %d\n", pid, flag);
	}
	wait(&status);
        return 0;
}

