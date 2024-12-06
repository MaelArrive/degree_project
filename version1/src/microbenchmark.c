#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "platform.h"
#include "xil_printf.h"
#include "xtime_l.h"
#include "xparameters.h"
#include "xtrafgen.h"
#include "xil_exception.h"
#include "xtrafgen_static_mode_example.h"

#define GLOBAL_LOOP 10


#define ARRAY_SIZE 1408
#define ITERATIONS 1000
#define COMPUTE_FACTOR 1000

#define MEM_LOCATION 0xB0012000U

#define TIMEDIFF(t1,t2)    (t2 - t1)
#define MILLISECONDS(t) (1000.0 * t / COUNTS_PER_SECOND)

#define ATG

XTime start, end;
double totalTime = 0.0;
XTrafGen XTrafGenInstance;

void compute_work(int factor);
void memory_stress_test(int *array, int size, int compute_factor);

void startTest();
void endTest();

int main(int argc, char *argv[]) {
    init_platform();

#ifdef ATG
	xil_printf("Configuration WITH ATG\r\n");
#else
	xil_printf("Configuration WITHOUT ATG\r\n");
#endif

    int *array;
    int size = ARRAY_SIZE;
    int compute_factor = COMPUTE_FACTOR;
    uintptr_t mem_location;
    mem_location = MEM_LOCATION;

    for(int i = 0; i<GLOBAL_LOOP; i++) {
		xil_printf("Loop number %d starting...\r\n", i);
		array = (int *)mem_location;

		// Initialize the array
		for (int i = 0; i < size; ++i) {
			array[i] = rand() % 100;
		}

#ifdef ATG
		int Status = InitializeXTrafGenStaticMode(&XTrafGenInstance);
		if (Status != XST_SUCCESS) {
			xil_printf("ATG Initialization failed\n\r");
			return Status;
		}

		Status = EnableXTrafGenStaticMode(&XTrafGenInstance);
		if (Status != XST_SUCCESS) {
			xil_printf("ATG Enable failed\n\r");
			return Status;
		}
#endif

		startTest();
		memory_stress_test(array, size, compute_factor);
		endTest();

#ifdef ATG
		Status = DisableXTrafGenStaticMode(&XTrafGenInstance);
		if (Status != XST_SUCCESS) {
			xil_printf("ATG Disabled failed\n\r");
			return Status;
		}
#endif
		if (!mem_location) {
			free(array);
		}
		sleep(5);
    }

    printf("The application ran successfully, the average run-time is %.2f msec\n", totalTime/GLOBAL_LOOP);
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
    printf("Run-time = %.2f msec...\n", msec);
}
