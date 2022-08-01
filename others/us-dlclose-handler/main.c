#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#define LIBPATH "./libcount.so"

#include <unistd.h>
#include <sys/syscall.h>

#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))

void *handler = NULL;
#define THREAD_NUM 2

pthread_t threads[THREAD_NUM];
int done[THREAD_NUM];

void *ThreadRun(void *);


void *ThreadRun(void *arg)
{
    if (handler == NULL || handler != 0)
    {
        handler = dlopen(LIBPATH, RTLD_LAZY);
        if (handler == NULL)
        {
            printf("test02........... \r\n");
            printf("ERROR:%s:dlopen\n", dlerror());
            return;
        }
    }
   
    //  if a shared library (.so) is loaded by dlopen, run this line
    if (handler > 0)
    {
        void (*inc)() = (void (*)())dlsym(handler, "inc");
        if (inc == NULL)
        {
            printf("ERROR:%s:dlsym\n", dlerror());
            return;
        }
    
        int (*get)() = (int (*)())dlsym(handler, "get");
        if (get == NULL)
        {
            printf("ERROR:%s:dlsym\n", dlerror());
            return;
        }
    
        for (int i = 0 ; i < 3; i++)
        {
            inc();
            printf("ThreadRun(): PID(%d), Result(%d)\n", gettid(), get());
            usleep(1000*2000);
        }
        dlclose(handler);
    }
}



int main(void)
{
	int i;
	int rc;
	int status;
	
	printf("pid=%d\n", getpid());
	
	for (i = 0; i < THREAD_NUM; i++)
	{	
		done[i] = 0;
		pthread_create(&threads[i], NULL, &ThreadRun, i);
		printf("%d, %ld\n", i, threads[i]);
	}
    printf("-------------------------\r\n");

	for (i = THREAD_NUM -1 ; i >= 0; i--)
	{
		done[i] = 1;
	         rc = pthread_join(threads[i], (void **)&status);
		if (rc == 0)
		{
			printf("Completed join with thread %d status= %d\n",i, status);
		}
		else
		{
			printf("ERROR; return code from pthread_join() is %d, thread %d\n", rc, i);
            return -1;
		}
	}
	return 0;
}