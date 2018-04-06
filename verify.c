#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_ptree      380


struct prinfo {
  long state;             /* current state of process */
  pid_t pid;              /* process id */
  pid_t parent_pid;       /* process id of parent */
  pid_t first_child_pid;  /* pid of oldest child */
  pid_t next_sibling_pid; /* pid of younger sibling */
  long uid;               /* user id of process owner */
  char comm[64];          /* name of program executed */
};

static void emit_buf(struct prinfo *buf, int nentry)
{
	struct prinfo *info;
	int counter;
	int depth=0;

	printf("Emit buf:\n");
	for(counter = 0; counter < nentry; counter++)
	{
		info = buf+counter;

		if(info == NULL)
			break;

		printf("Access info(%p) at buf(%p) + counter(%d)\n", info, buf, counter);
		int _i = depth;
		while(_i)
		{
			printf("	");
			_i--;
		}


		if(counter != (nentry-1))
		{

			// parent goes down to child
			if(info->first_child_pid == (info+1)->pid)	
				depth++;
			// child goes to sibling
			else if(info->parent_pid == (info+1)->parent_pid)
				;
			// child goes to parent
			if(info->parent_pid == (info+1)->pid)
				depth--;
		}
		printf("%s,%d,%ld,%d,%d,%d,%ld\n", info->comm, info->pid, info->state,  info->parent_pid, info->first_child_pid, info->next_sibling_pid, info->uid);
	}
}

int main(int args, char **argv)
{
	if(args!=2)
	{
		printf("Please enter how many entries you would like:\n");
		exit(1);
	}
	int N_ENTRY;
	sscanf(argv[1], "%d", &N_ENTRY);
	printf("got a %d\n", N_ENTRY);



        long flag;
        struct prinfo *pool =(struct prinfo *) malloc(N_ENTRY*sizeof(struct prinfo));
	int *nr = malloc(sizeof(int));


        *nr = N_ENTRY;


        flag = syscall(__NR_ptree, pool, nr);
	// The pool shall be filled up after ptree syscall
        printf("System call invoked with total number of process: %ld\n", flag);
	
	emit_buf(pool, N_ENTRY);

	free(pool);
        return 0;
}

