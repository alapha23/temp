#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_rotlock_read	381
#define __NR_rotlock_write	382
#define __NR_rotunlock_read	383
#define __NR_rotunlock_write	313

int main(int argc, char **argv)
{
	int ret;
        ret = syscall(__NR_rotlock_read, NULL, NULL);
	if(ret != 0)	printf("lock read %d return %d\n", __NR_rotlock_read, ret);


        ret = syscall(__NR_rotunlock_read, NULL, NULL);
	if(ret != 0)	printf("unlock read %d return %d\n", __NR_rotunlock_read, ret);

        ret = syscall(__NR_rotlock_write, NULL, NULL);
	if(ret != 0)	printf("writer lock %d return %d\n", __NR_rotlock_write, ret);

        ret = syscall(__NR_rotunlock_write, NULL, NULL);
	if(ret != 0)	printf("writer unlock %d return %d\n", __NR_rotunlock_write, ret);

	return 0;
}
