#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xtime_l.h"	// XTime_GetTime()

#define CLS 32
#define SM (CLS / sizeof(float)) // Submatrix size
#define DIM 512 // Adjust dimension as needed for testing
#define TIMEDIFF(t1,t2)	(t2 - t1)
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

void Multiply_New(float **A, float **B, float **Res, long dim) {
    for (int i = 0; i < dim; i += SM) {
        for (int j = 0; j < dim; j += SM) {
            for (int k = 0; k < dim; k += SM) {
                float *rres = &Res[i][j];
                float *rmul1 = &A[i][k];
                for (int i2 = 0; i2 < SM; ++i2, rres += dim, rmul1 += dim) {
                    float *rmul2 = &B[k][j];
                    for (int k2 = 0; k2 < SM; ++k2, rmul2 += dim) {
                        for (int j2 = 0; j2 < SM; ++j2) {
                            rres[j2] += rmul1[k2] * rmul2[j2];
                        }
                    }
                }
            }
        }
    }
}

void initialize_matrix(float **matrix, long dim) {
    if (matrix == NULL) {
    	print("Error: Matrix is NULL!\n");
        return;  // Exit if the matrix is NULL
    }

    // Loop through all elements of the matrix to initialize them
    for (int i = 0; i < dim; i++) {
        if (matrix[i] == NULL) {
        	printf("Error: Row %d is NULL!\n", i);
            return;  // Exit if a row is NULL
        }
        for (int j = 0; j < dim; j++) {
            matrix[i][j] = (float)(rand() % 10);  // Assign a random value between 0 and 9
        }
    }
}

float **allocate_matrix(long dim) {
    float **matrix = (float **)malloc(dim * sizeof(float *));
    for (int i = 0; i < dim; i++) {
        matrix[i] = (float *)calloc(dim, sizeof(float));
    }
    return matrix;
}

void free_matrix(float **matrix, long dim) {
    for (int i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    for(int i = 0; i<10; i++){
        init_platform();
        printf("The platform is ready for iteration %d!\n\r", i);

        float **A = allocate_matrix(DIM);
        float **B = allocate_matrix(DIM);
        float **Res = allocate_matrix(DIM);

        initialize_matrix(A, DIM);
        initialize_matrix(B, DIM);

        startTest();
        Multiply_New(A, B, Res, DIM);
    	endTest();

        free_matrix(A, DIM);
        free_matrix(B, DIM);
        free_matrix(Res, DIM);

        print("The iteration ran successfully!\n\r");
    	cleanup_platform();
    }

    print("The application ran successfully!\n\r");
	cleanup_platform();

    return 0;
}
