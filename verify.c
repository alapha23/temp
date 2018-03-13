#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_ptree      380
#define N_ENTRY         200


struct prinfo {
  long state;             /* current state of process */
  pid_t pid;              /* process id */
  pid_t parent_pid;       /* process id of parent */
  pid_t first_child_pid;  /* pid of oldest child */
  pid_t next_sibling_pid; /* pid of younger sibling */
  long uid;               /* user id of process owner */
  char comm[64];          /* name of program executed */
  int  depth;
};

static void emit_buf(struct prinfo *buf, int entry)
{
	struct prinfo *info;
	int counter;

	for(counter = 0; counter < entry; counter++)
	{
		info = buf+counter;
		int dep = info->depth;
		while(dep)
		{
			printf("	");
			dep--;
		}
		printf("%s,%d,%ld,%d,%d,%d,%ld\n", info->comm, info->pid, info->state,  info->parent_pid, info->first_child_pid, info->next_sibling_pid, info->uid);

	}
}

int main(void)
{
        long flag;

        struct prinfo *pool =(struct prinfo *) malloc(N_ENTRY*sizeof(struct prinfo));
	int *nr = malloc(sizeof(int));
        *nr = N_ENTRY;


        flag = syscall(__NR_ptree, pool, nr);
	// The pool shall be filled up after ptree syscall
	// TODO: Print out pool right here
        printf("System call invoked with flag %ld\n", flag);
	
	emit_buf(pool, N_ENTRY);
        return 0;
}

