#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define THRESHOLD       5
#define MAX_THREAD      2

pthread_t tid1;
pthread_t tid[MAX_THREAD];
int counter;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct
{
    char name[16];
    char msg[64];
}thread_args;

void *threadFunc1(void *argv)
{
    Q_UNUSED(argv);
    pthread_mutex_lock(&mutex);
    printf("pthread has started\n");

    while(counter < THRESHOLD)
    {
        counter += 1;
        sleep(5);
    }

    pthread_cond_signal(&cond);
    printf("pthread has finished, counter = %d\n", counter);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void *threadFunc(void *argv)
{
    Q_UNUSED(argv);
    pthread_mutex_lock(&mutex);
    counter += 1;
    printf("Thread %d has started\n", counter);

    sleep(1);
    printf("Thread %d has finished\n", counter);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void *thread_start_detach(void *args)
{
    Q_UNUSED(args);
    if(pthread_detach(pthread_self()))
    {
        printf("pthread_detach error\n");
    }
    pthread_exit(NULL);
}

void *thread_start(void *args)
{
    Q_UNUSED(args);
    pthread_exit(NULL);
}

static void *printHello(void *args)
{
    thread_args *thr_args = (thread_args*)args;
    printf ("Hello World! I am: %s!\n", thr_args->name);
    printf ("%s!\n", thr_args->msg);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(false){
        pthread_t threadID;
        int ret;
        thread_args thr;

        memset(&thr, 0x0, sizeof(thread_args));
        strncpy(thr.name, "trung", sizeof(thr.name));

        ret = pthread_create(&threadID, NULL, printHello, (&thr));
        if(ret)
        {
            printf("pthread_create() error number is %d\n", ret);
            return -1;
        }
        pthread_exit(NULL);
    }

    if(false){
        pthread_t threadID;
        int ret;
        long count = 0;
        void *retval;

        while(1)
        {
            ret = pthread_create(&threadID, NULL, thread_start, NULL);
            if(ret)
            {
                printf("pthread_create() fail ret: %d\n", ret);
                perror("Fail reason:");
                break;
            }
            pthread_join(threadID, &retval);
            count++;
            if (0 == count %10000)
            {
                printf("Number of threads are created:%ld\n", count);
            }
        }
        printf("Number of threads are created:%ld\n", count);
    }

    if(false){
        pthread_t threadID;
        int ret;
        long count = 0;

        while(1)
        {
            ret = pthread_create(&threadID, NULL, thread_start_detach, NULL);
            if(ret)
            {
                printf("pthread_create() fail ret: %d\n", ret);
                perror("Fail reason:");
                break;
            }
            usleep(10);
            count++;
            if (0 == count %10000)
            {
                printf("Number of threads are created:%ld\n", count);
            }
        }
        printf("Number of threads are created:%ld\n", count);
        return 0;
    }

    if(false){
        int i = 0;
        int ret = 0;

        for (i = 0; i < MAX_THREAD; i++)
        {
            ret = pthread_create(&(tid[i]), NULL, threadFunc, NULL);
            if (ret != 0)
            {
                printf("Thread [%d] created error\n", i);
            }
        }

        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
    }

    if(true){
        int ret = 0;

        ret = pthread_create(&tid1, NULL, &(threadFunc1), NULL);
        if (ret != 0)
        {
            printf("pthread created fail\n");
        }

        pthread_mutex_lock(&mutex);
        while(counter < THRESHOLD)
        {
            pthread_cond_wait(&cond, &mutex);
            printf("Global variable counter = %d.\n", counter);
        }

        pthread_mutex_unlock(&mutex);
        pthread_join(tid1, NULL);
    }

    return a.exec();
}
