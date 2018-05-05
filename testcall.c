#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_set_rotation	380
#define __NR_rotlock_read	381
#define __NR_rotlock_write	382
#define __NR_rotunlock_read	383
#define __NR_rotunlock_write	313

int main(int argc, char **argv)
{
	int ret;
        ret = syscall(__NR_set_rotation, 15);

        ret = syscall(__NR_rotlock_read, 15, 5);
printf("Read lock1 \n");

        ret = syscall(__NR_set_rotation, 100);

//	printf("lock read %d return %d\n", __NR_rotlock_read, ret);

        ret = syscall(__NR_rotlock_read, 100, 20);
printf("Read lock2 \n");


        ret = syscall(__NR_rotunlock_read, 15, 5);
        ret = syscall(__NR_rotunlock_read, 100, 20);

	//printf("unlock read %d return %d\n", __NR_rotunlock_read, ret);


//	sleep(1);
//        ret = syscall(__NR_rotlock_write, NULL, NULL);
//	printf("writer lock %d return %d\n", __NR_rotlock_write, ret);

  //      ret = syscall(__NR_rotunlock_write, NULL, NULL);
//	printf("writer unlock %d return %d\n", __NR_rotunlock_write, ret);

	return 0;
}
