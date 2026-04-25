#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>

#define N 500  // maximum matrix size

// Function to generate random matrix
void generateMatrix(int n, int **A) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            A[i][j] = rand() % 10;
}

// Function to print matrix
void printMatrix(int n, int **A) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }
}
void printSharedMatrix(int n, int (*A)[n]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }
}

int main() {
    srand(time(NULL));
    int n;
    printf("Enter matrix limit:");
    scanf("%d",&n);
    //int n = N;
    int **matA = malloc(n * sizeof(int *));
    int **matB = malloc(n * sizeof(int *));
    int **matC_serial=malloc(n*sizeof(int *));

    for (int i = 0; i < n; i++) {
        matA[i] = malloc(n * sizeof(int));
        matB[i] = malloc(n * sizeof(int));
        matC_serial[i] = malloc(n * sizeof(int));
     }

    printf("Matrix size: %dx%d\n\n", n, n);

    generateMatrix(n, matA);
    generateMatrix(n, matB);

    if(n<=5){
       printf("Matrix A:\n");
       printMatrix(n, matA);
       printf("\nMatrix B:\n");
       printMatrix(n, matB);
    }

    struct timespec start, end;

    // ================= SERIAL EXECUTION =================
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matC_serial[i][j] = 0;
            for (int k = 0; k < n; k++) {
                if(n<=5){
                printf("Serial: C[%d][%d] += A[%d][%d] * B[%d][%d]\n",
                        i, j, i, k, k, j);
                }
                matC_serial[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

  clock_gettime(CLOCK_MONOTONIC, &end);
        double serial_time =
	           (end.tv_sec - start.tv_sec)*1000 +
		           (end.tv_nsec - start.tv_nsec) / 1000000;
   
    if(n<=5){
       printf("\nSerial Result Matrix:\n");
       printMatrix(n, matC_serial);
    }

    // ================= PARALLEL EXECUTION =================
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * n * n, IPC_CREAT | 0666);
    int (*matC_parallel)[n] = shmat(shmid, NULL, 0);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            for (int j = 0; j < n; j++) {
                matC_parallel[i][j] = 0;
                for (int k = 0; k < n; k++) {
                    if(n<=5){
                        printf("Child %d: C[%d][%d] += A[%d][%d] * B[%d][%d]\n",
                            getpid(), i, j, i, k, k, j);
                       }
                        matC_parallel[i][j] += matA[i][k] * matB[k][j];
                }
            }
            exit(0);
        }
    }

    for (int i = 0; i < n; i++)
        wait(NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double parallel_time =
        (end.tv_sec - start.tv_sec)*1000 +
        (end.tv_nsec - start.tv_nsec) / 1000000;

    if(n<=5){
       printf("\nParallel Result Matrix:\n");
       printSharedMatrix(n, matC_parallel);
    }
    printf("\nExecution Time:\n");
    printf("Serial Time   : %lf seconds\n", serial_time);
    printf("Parallel Time : %lf seconds\n", parallel_time);

    shmdt(matC_parallel);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
