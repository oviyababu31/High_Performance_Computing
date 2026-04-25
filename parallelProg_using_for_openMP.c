#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main(/*int argc, char *argv[]*/) {
    int n = 1000000;
    int *A, *B, *C_serial, *C_parallel;
    int thread_count=4;
    
  /*  if (argc < 2) {
        printf("Usage: %s <thread_count>\n", argv[0]);
        return 1;
    }

    thread_count = atoi(argv[1]);*/

    A = (int*) malloc(n * sizeof(int));
    B = (int*) malloc(n * sizeof(int));
    C_serial = (int*) malloc(n * sizeof(int));
    C_parallel = (int*) malloc(n * sizeof(int));
    int i;
    srand(time(0));

    for(i = 0; i < n; i++) {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
    }

    FILE *fp = fopen("result_program1.txt", "w");

    // 🔹 SERIAL
    double serial_start = omp_get_wtime();

    for( i = 0; i < n; i++) {
        C_serial[i] = A[i] + B[i];
    }

    double serial_end = omp_get_wtime();

    // 🔹 PARALLEL
    double parallel_start = omp_get_wtime();

    #pragma omp parallel num_threads(thread_count)
    {
        int tid = omp_get_thread_num();
        double t_start = omp_get_wtime();

        #pragma omp for
        for( i = 0; i < n; i++) {
            C_parallel[i] = A[i] + B[i];
        }

        double t_end = omp_get_wtime();
        printf("Thread %d time: %f seconds\n", tid, t_end - t_start);
    }

    double parallel_end = omp_get_wtime();

    double serial_time = serial_end - serial_start;
    double parallel_time = parallel_end - parallel_start;

    printf("\nSerial Time: %f seconds\n", serial_time);
    printf("Parallel Time: %f seconds\n", parallel_time);
    printf("Speedup: %f\n", serial_time / parallel_time);

    // 🔹 Write to file
    fprintf(fp, "operations performed::\n");
    for( i = 0; i < n; i++) {
        fprintf(fp, "%d + %d = %d\n", A[i], B[i], C_parallel[i]);
    }

    fprintf(fp, "\nSerial Time: %f\nParallel Time: %f\nSpeedup: %f\n",
            serial_time, parallel_time, serial_time / parallel_time);

    fclose(fp);

    free(A); free(B); free(C_serial); free(C_parallel);
    return 0;
}