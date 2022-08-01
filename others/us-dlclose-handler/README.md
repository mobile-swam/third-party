

## How to run
Multi-process in C/C++ utilizes dlopen, dlsym, and dlclose to load dynamic libraries.
I once mistakenly believed that dynamic libraries are loaded into memory (operating system).
If multiple processes simultaneously dlopen the same dynamic library, there should be only one dynamic library instance in the operating system.
Obviously, **global variables** in the dynamic library are also Only one variables.

In fact, dynamic libraries are loaded into distinct process spaces.
And then, distinct instances of the same dynamic library are opened concurrently by distinct processes.

```bash
$ bash ./run.sh                                                                                                                                                                                                                                                         (culint5)  2022년 08월 01일 (월) 오후 06시 07분 53초
main.c: In function ‘ThreadRun’:
main.c:36:13: warning: ‘return’ with no value, in function returning non-void
             return;
             ^~~~~~
main.c:27:7: note: declared here
 void *ThreadRun(void *arg)
       ^~~~~~~~~
main.c:47:13: warning: ‘return’ with no value, in function returning non-void
             return;
             ^~~~~~
main.c:27:7: note: declared here
 void *ThreadRun(void *arg)
       ^~~~~~~~~
main.c:54:13: warning: ‘return’ with no value, in function returning non-void
             return;
             ^~~~~~
main.c:27:7: note: declared here
 void *ThreadRun(void *arg)
       ^~~~~~~~~
main.c: In function ‘main’:
main.c:80:49: warning: passing argument 4 of ‘pthread_create’ makes pointer from integer without a cast [-Wint-conversion]
   pthread_create(&threads[i], NULL, &ThreadRun, i);
                                                 ^
In file included from main.c:3:0:
/usr/include/pthread.h:234:12: note: expected ‘void * restrict’ but argument is of type ‘int’
 extern int pthread_create (pthread_t *__restrict __newthread,
            ^~~~~~~~~~~~~~
pid=15675
0, 139627231037184
1, 139627222644480
-------------------------
ThreadRun(): PID(15676), Result(1)
ThreadRun(): PID(15677), Result(2)
ThreadRun(): PID(15676), Result(3)
ThreadRun(): PID(15677), Result(4)
ThreadRun(): PID(15676), Result(5)
ThreadRun(): PID(15677), Result(6)
Completed join with thread 0 status= 0

```