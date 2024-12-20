#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "platform.h"
#include "xil_printf.h"
#include "xtime_l.h"
#include "xparameters.h"
#include "xtrafgen.h"
#include "xil_exception.h"
#include "xil_cache.h"
#include "ddr_qos_setup.h"


#define ARRAY_SIZE 4096
#define ITERATIONS 10
#define COMPUTE_FACTOR 20

#define TIMEDIFF(t1,t2)    (t2 - t1)
#define MILLISECONDS(t) (1000.0 * t / COUNTS_PER_SECOND)
#define EXTRACT_NIBBLE(value, index) ((Xil_In32(value) >> (index * 4)) & 0xF)

XTime start, end;
double totalTime = 0.0;
XTrafGen XTrafGenInstance;

void compute_work(int factor);
void memory_stress_test(int *array, int size, int compute_factor);

void startTest();
void endTest();

int main(int argc, char *argv[]) {
    init_platform();

    Xil_DCacheDisable();
    Xil_ICacheDisable();

    ddr_qos_setup();
//
//    int *array;
//    int size = ARRAY_SIZE;
//    int compute_factor = COMPUTE_FACTOR;
//
//    sleep(10);
//
//    for(int i = 0; i<10; i++){
//        printf("Iteration %d: ", i);
//		array = (int *)malloc(size * sizeof(int));
//		if (!array) {
//			fprintf(stderr, "Memory allocation failed\n");
//			return -1;
//		}
//
//		for (int i = 0; i < size; ++i) {
//			array[i] = rand() % 100;
//		}
//
//		startTest();
//		memory_stress_test(array, size, compute_factor);
//		endTest();
//
//		free(array);
//    }
//
//    printf("%.2f\n", totalTime/10);
    printf("The application ran successfully.\n");

    cleanup_platform();
    return 0;
}

void compute_work(int factor) {
    volatile int sum = 0;
    for (int i = 0; i < factor * 1000; ++i) {
        sum += i;
    }
}

void memory_stress_test(int *array, int size, int compute_factor) {
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        for (int i = 0; i < size; i += 64) {
            array[i] = array[i] + 1;
        }

        compute_work(compute_factor);
    }
}

void startTest()
{
    XTime_GetTime(&start);
}

void endTest()
{
    XTime_GetTime(&end);
    double time_curr = TIMEDIFF(start, end);
    double msec = MILLISECONDS(time_curr);
    totalTime = totalTime + msec;
    printf("%.2f\n", msec);
}
