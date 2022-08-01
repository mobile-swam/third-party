#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM 10
#define LIBPATH "./libcount.so"

void *ThreadRun(void *arg)
{
    printf("test............... \r\n");
    void *handler = dlopen(LIBPATH, RTLD_LAZY);
    if (handler == NULL)
    {
        printf("ERROR:%s:dlopen\n", dlerror());
        return;
    }
    printf("test2............... \r\n");
    void (*inc)() = (void (*)())dlsym(handler, "inc");
    if (inc == NULL)
    {
        printf("ERROR:%s:dlsym\n", dlerror());
        return;
    }

    printf("test3............... \r\n");
    int (*get)() = (int (*)())dlsym(handler, "get");
    if (get == NULL)
    {
        printf("ERROR:%s:dlsym\n", dlerror());
        return;
    }

    printf("test4............... \r\n");
    int i = 0;
    for (; i < NUM; i++)
    {
        inc();
        usleep(1000*1000);
        printf("ThreadRun:PID(%d):%d\n", getpid(), get());
    }

    dlclose(handler);
}

void *ThreadRun2(void *arg)
{
    void *handler = dlopen(LIBPATH, RTLD_LAZY);
    if (handler == NULL)
    {
        printf("ERROR:%s:dlopen\n", dlerror());
        return;
    }
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

    int i = 0;
    for (; i < NUM; i++)
    {
        inc();
        usleep(1000*1000);
        printf("ThreadRun2: PID(%d):%d\n", getpid(), get());
    }

    dlclose(handler);
}

int main()
{
    pthread_t tid, tid2;
    tid = 0, tid2=0;
    int status; 

    for (int i = 1 ; i <= 1 ; i++)
    {   tid++;
        pthread_create(&tid, NULL, ThreadRun, NULL);
        printf("create ThreadRun OK, %d !!!\n", tid);
        pthread_join(tid, (void **)&status);
   
        tid2++; 
        pthread_create(&tid2, NULL, ThreadRun2, NULL);
        printf("create ThreadRun2 OK, %d !!!\n", tid2);
        pthread_join(tid, (void **)&status);
    }
    //while (1);

    return 0;
}