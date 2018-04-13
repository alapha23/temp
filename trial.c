#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <math.h>

#define __NR_rotlock_read       381
#define __NR_rotlock_write      382
#define __NR_rotunlock_read     383
#define __NR_rotunlock_write    313

/*
 * 
 * 
*/

FILE *fp;
long long integer;

void open_file(void)
{
        if((fp =fopen("integer", "r")) == NULL)
        {
                fprintf(stderr, "filedir: %s\n", "integer");
                fflush(stderr);
                exit(1);
        }
}

// A function to print all prime factors of a given number n
void primeFactors(long long n)
{
       while(n % 2 == 0)
        {
                printf("%d ", 2);
                n = n / 2;
                if(n > 2)
                printf("* ");
        }

        for(int i = 3; i <= sqrt(n); i = i + 2)
        {
                while(n % i == 0)
                {
                        printf("%d ", i);
                        n = n / i;
                        if(n > 2)
                        printf("* ");
                }
        }
        if(n > 2)
	printf("%lld \n", n);
}


int main(int argc, char**argv)
{
	char line[128];
	int range = 90;
	int degree = 90; 
	if(argc == 1)
	{
		fputs("Usage: ./trialor <ident>\n", stderr);
		exit(1);
	}

	while(1)
	{
		open_file();
		syscall(__NR_rotlock_read, degree, range);

		if (fgets(line, 64, fp) == NULL)
		{
			perror("Read error\n");
			exit(1);
		}
		integer = atoi(line);
		printf("trial-%s: %lld = ", argv[1], integer);
		primeFactors(integer);
		syscall(__NR_rotunlock_read, degree, range);

		fclose(fp);
	}
	return 0;
}
