#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "xil_printf.h"
#include "xtime_l.h"    // XTime_GetTime()

#define CLS 32
#define SM (CLS / sizeof(float)) // Submatrix size
#define DIM 512 // Adjust dimension as needed for testing
#define TIMEDIFF(t1,t2)    (t2 - t1)
#define MILLISECONDS(t) (1000.0 * t / COUNTS_PER_SECOND)

// Globals
XTime start, end;

// Start a test
void startTest()
{
    XTime_GetTime(&start);
}

// End a test
void endTest()
{
    XTime_GetTime(&end);
    double time_curr = TIMEDIFF(start, end);
    double msec = MILLISECONDS(time_curr);
    printf("Run-time = %.2f msec...\n", msec);
}

void Multiply(float **A, float **B, float **Res, long dim)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			for (int Index = 0; Index < dim; Index++)
			{
				Res[i][j] += A[i][Index] * B[Index][j];
			}
		}
	}
}

void initialize_matrix(float **matrix, long dim) {
    if (matrix == NULL) {
    	xil_printf("Error: Matrix is NULL!\r\n");
        return;  // Exit if the matrix is NULL
    }

    // Loop through all elements of the matrix to initialize them
    for (int i = 0; i < dim; i++) {
        if (matrix[i] == NULL) {
        	xil_printf("Error: Row %d is NULL!\r\n", i);
            return;  // Exit if a row is NULL
        }
        for (int j = 0; j < dim; j++) {
            matrix[i][j] = (float)(rand() % 10);  // Assign a random value between 0 and 9
        }
    }
}

float **allocate_matrix(long dim) {
    float **matrix = (float **)malloc(dim * sizeof(float *));
    if (matrix == NULL) {
        xil_printf("Memory allocation failed!\r\n");
        exit(1);  // or handle the error appropriately
    }
    for (int i = 0; i < dim; i++) {
        matrix[i] = (float *)calloc(dim, sizeof(float));
        if (matrix[i] == NULL) {
            xil_printf("Memory allocation failed for row %d!\r\n", i);
            exit(1); // Handle error appropriately
        }
    }
    return matrix;
}

void free_matrix(float **matrix, long dim) {
    for (int i = 0; i < dim; i++) {
        free(matrix[i]);
        matrix[i] = NULL; // Avoid double-free
    }
    free(matrix);
    matrix = NULL; // Avoid double-free
}

int main() {
    init_platform();
    print("The application is ready!\n\r");

    for (int i = 0; i < 10; i++) {
    	sleep(10);
        xil_printf("Iteration number %d starting...\n\r", i+1);
		float **A = allocate_matrix(DIM);
		float **B = allocate_matrix(DIM);
		float **Res = allocate_matrix(DIM);

		if (A == NULL || B == NULL || Res == NULL) {
			xil_printf("Matrix allocation failed!\n\r");
			return 1;
		}

		initialize_matrix(A, DIM);
		initialize_matrix(B, DIM);

		startTest();
		Multiply(A, B, Res, DIM);
		endTest();

		free_matrix(A, DIM);
		free_matrix(B, DIM);
		free_matrix(Res, DIM);
        xil_printf("Iteration number %d ended!\n\r", i+1);
    }
    print("The application ran successfully!\n\r");
    cleanup_platform();

    return 0;
}
