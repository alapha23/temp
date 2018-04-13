#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_rotlock_read       381
#define __NR_rotlock_write      382
#define __NR_rotunlock_read     383
#define __NR_rotunlock_write    313

int main(int argc, char **argv)
{

 int ret;
        if(argc < 2)
        {
                printf("./testwriter1 <identifier\n");
                exit(1);
        }

        while(1)
        {
                ret = syscall(__NR_rotlock_write, NULL, NULL);
                printf("writer%s %d return %d\n", argv[1], __NR_rotlock_read, ret);
                ret = syscall(__NR_rotunlock_write, NULL, NULL);

                printf("writer%s unlock %d return %d\n", argv[1], __NR_rotlock_read, ret);
        }

        
}

