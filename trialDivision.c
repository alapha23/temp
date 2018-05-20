#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <errno.h>              /* errno and error codes */
#include <sys/time.h>           /* for gettimeofday() */
#include <unistd.h>             /* for gettimeofday(), getpid() */
#include <stdio.h>              /* for printf() */
#include <unistd.h>             /* for fork() */
#include <sys/types.h>          /* for wait() */
#include <sys/wait.h>           /* for wait() */
#include <signal.h>             /* for kill(), sigsuspend(), others */
#include <sys/ipc.h>            /* for all IPC function calls */
#include <sys/shm.h>            /* for shmget(), shmat(), shmctl() */
#include <sys/sem.h>            /* for semget(), semop(), semctl() */
#include <sched.h>              /* for scheduling calls/constants */
#include <sys/resource.h>


#define sched_setweight 380
#define SCHED_WRR	6

void trialDivision(int n) {
	int i = 2;
	while (n > 1) {
		if (n%i==0) {
			//printf("%d ",i);
			n = n/i;
		}
		else {
			i++;
		}
	}
	//printf("\n");
}

int main(int argc, char **argv) {
	/*gnuplot*/
/*	FILE *gnuplot = popen("gnuplot -persist", "w");
	fprintf(gnuplot, "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5\n");
	fprintf(gnuplot, "plot '-' with linespoints ls 1\n");
*/
	struct sched_param param;
	int pid;
	int status;
        param.sched_priority=0;
        int flag = sched_setscheduler( getpid(), SCHED_WRR, &param);
	if(argc < 2)
	{
		printf("Enter prime number as argument \n");
		exit(0);
	}

	printf("Weight 		N	Time\n");

	for (int weight=1; weight<=20; weight++) {
		flag = syscall(sched_setweight, getpid(), weight);
		
		if((pid=fork())!=0)
		{
		clock_t begin = clock();
		int n = atoi(argv[1]);
		for (int i=2; i<=n; i++) {
			//printf("n = %d\n", i);
			trialDivision(i);
			//printf("\n");
		}
		clock_t end = clock();
		double time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
		
//	fprintf(gnuplot, "%d %g\n", weight, time_spent); //gnuplot
		
		printf("%d 	%d	 %f\n", weight, n, time_spent);
		exit(0);
		}
		wait(&status);
	}
	
	/*gnuplot*/
//	fprintf(gnuplot, "e\n");
//	fflush(gnuplot);
}
