#include "count.h"

int get()
{
    return count;
}

void inc()
{
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
}