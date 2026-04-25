#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define SIZE 20
#define MAXLINE 100

char buffer[SIZE][MAXLINE];
int count = 0;
int finished_producers = 0;

omp_lock_t lock;

int main() {
    int producers = 2;
    int consumers = 2;

    omp_init_lock(&lock);

    #pragma omp parallel num_threads(producers + consumers)
    {
        int id = omp_get_thread_num();
        if (id < producers) {
            char fname[20];
            sprintf(fname, "file%d.txt", id + 1);
            FILE *fp = fopen(fname, "r");

            if (fp == NULL) {
                printf("Error: Cannot open %s\n", fname);
            } else {
                char line[MAXLINE];

                while (fgets(line, MAXLINE, fp)) {
                    int inserted = 0;
                    while (!inserted) {
                        omp_set_lock(&lock);
                        if (count < SIZE) {
                            strcpy(buffer[count], line);
                            #pragma omp atomic
                            count++;
                            inserted = 1;
                        }
                        omp_unset_lock(&lock);
                    }
                }
                fclose(fp);
            }

            #pragma omp critical
            finished_producers++;
        }
        else {
            while (finished_producers < producers || count > 0) {
                char line[MAXLINE];
                int taken = 0;

                while (!taken) {
                    omp_set_lock(&lock);
                    if (count > 0) {
                        #pragma omp atomic
                        count--;
                        strcpy(line, buffer[count]);
                        taken = 1;
                    }
                    omp_unset_lock(&lock);
                }

     
                char word[50];
                int i = 0;
int j;
                for (j = 0; line[j] != '\0'; j++) {
                    if (line[j] == ' ' || line[j] == '\t' || line[j] == '\n') {
                        if (i > 0) {
                            word[i] = '\0';
                            printf("Consumer %d: %s\n", id, word);
                            i = 0;
                        }
                    } else {
                        word[i++] = line[j];
                    }
                }

                if (i > 0) {
                    word[i] = '\0';
                    printf("Consumer %d: %s\n", id, word);
                }
            }
        }
    }

    omp_destroy_lock(&lock);
    return 0;
}
