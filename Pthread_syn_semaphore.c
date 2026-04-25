#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5

typedef struct
{
    int a;
    int b;
    char op;
} task;

task work_buffer[BUFFER_SIZE];

int in = 0;
int out = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

FILE *fp;

/* Addition Thread */
void* add_thread(void *arg)
{
    int cnt=0;
    while(cnt<5)
    {
        
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        task t = work_buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        cnt; 
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if(t.op == '+')
        {
            int result = t.a + t.b;
            printf("Addition Thread: %d + %d = %d\n",t.a,t.b,result);
            fprintf(fp,"%d + %d = %d\n",t.a,t.b,result);
        }
    }
}

/* Subtraction Thread */
void* sub_thread(void *arg)
{
    while(1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        task t = work_buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if(t.op == '-')
        {
            int result = t.a - t.b;
            printf("Subtraction Thread: %d - %d = %d\n",t.a,t.b,result);
            fprintf(fp,"%d - %d = %d\n",t.a,t.b,result);
        }
    }
}

/* Multiplication Thread */
void* mul_thread(void *arg)
{
    while(1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        task t = work_buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if(t.op == '*')
        {
            int result = t.a * t.b;
            printf("Multiplication Thread: %d * %d = %d\n",t.a,t.b,result);
            fprintf(fp,"%d * %d = %d\n",t.a,t.b,result);
        }
    }
}

/* Division Thread */
void* div_thread(void *arg)
{
    while(1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        task t = work_buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if(t.op == '/' && t.b != 0)
        {
            float result = (float)t.a / t.b;
            printf("Division Thread: %d / %d = %.2f\n",t.a,t.b,result);
            fprintf(fp,"%d / %d = %.2f\n",t.a,t.b,result);
        }
    }
}

int main()
{
    pthread_t t1,t2,t3,t4;

    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);

    pthread_mutex_init(&mutex,NULL);

    fp = fopen("result_file.txt","w");

    pthread_create(&t1,NULL,add_thread,NULL);
    pthread_create(&t2,NULL,sub_thread,NULL);
    pthread_create(&t3,NULL,mul_thread,NULL);
    pthread_create(&t4,NULL,div_thread,NULL);

    char ops[] = {'+','-','*','/'};

    srand(time(NULL));

    while(1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        task t;
        t.a = rand()%100;
        t.b = rand()%50 + 1;
        t.op = ops[rand()%4];

        work_buffer[in] = t;

        printf("Server Generated: %d %c %d\n",t.a,t.op,t.b);

        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);
    }

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);

    sem_destroy(&empty);
    sem_destroy(&full);

    fclose(fp);

    return 0;
}
