#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_rotlock_read       381
#define __NR_rotlock_write      382
#define __NR_rotunlock_read     383
#define __NR_rotunlock_write    313

/*
 * Take write lock when 0~180
 * Write to integer file
 * release write lock
*/

FILE *fp;
long long integer;

void open_file(void)
{
        if((fp =fopen("integer", "w")) == NULL)
        {
                fprintf(stderr, "filedir: %s\n", "integer");
                fflush(stderr);
                exit(1);
        }
}


int main(int argc, char *argv[])
{
   int range = 90;
   int degree = 90; 

   if(argc == 1)
   {
      fputs("Usage: ./selector <number>\n", stderr);
      exit(1);
   }

   integer =(long long) atoi(argv[1]);

   open_file();   
   while(1)
   {   
      fflush(fp);
      fseek(fp, sizeof(long long), SEEK_SET);

      syscall(__NR_rotlock_write, degree, range);
      fprintf(fp, "%lld", integer);
      fflush(fp);
      printf("selector: %lld\n", integer);
      integer = integer + 1;
      syscall(__NR_rotunlock_write, degree, range);

   }
   
   fclose(fp);
   return 0;
}
