#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <errno.h>		/* errno and error codes */
#include <sys/time.h>		/* for gettimeofday() */
#include <unistd.h>		/* for gettimeofday(), getpid() */
#include <stdio.h>		/* for printf() */
#include <unistd.h>		/* for fork() */
#include <sys/types.h>		/* for wait() */
#include <sys/wait.h>		/* for wait() */
#include <signal.h>		/* for kill(), sigsuspend(), others */
#include <sys/ipc.h>		/* for all IPC function calls */
#include <sys/shm.h>		/* for shmget(), shmat(), shmctl() */
#include <sys/sem.h>		/* for semget(), semop(), semctl() */
#include <sched.h>		/* for scheduling calls/constants */
#include <sys/resource.h>

#define SCHED_RR		2
#define SCHED_WRR		6

#define __NR_setscheduler	156
#define __NR_sched_setweight      380
#define __NR_sched_getweight      381

int main(int args, char **argv)
{

        int flag;
	int weight;
	int pid;
	int status;
	struct sched_param param;

	param.sched_priority=0;

	if((pid=fork())!=0)
	{
	        flag = sched_setscheduler( pid, SCHED_WRR, &param);
//	        flag = syscall(__NR_setscheduler, pid, &param);
	        printf("SET SCHEDULER: Process %d returned: %d \n", pid, flag);
		int counter=500;
		while(counter)
		{
			weight = 11;
		        flag = syscall(__NR_sched_setweight, pid, weight);
		        printf("SET: Process %d weight returned: %d \n", pid, flag);
			flag = syscall(__NR_sched_getweight, pid, weight);
		        printf("GET: Process %d has weight %d\n", pid, flag);
			counter--;
		}
	}
	wait(&status);
        return 0;
}

