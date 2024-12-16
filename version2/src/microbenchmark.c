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
#include "axiqos.h"
#include "apuqos.h"

#define QOS_HP_R 0x0
#define QOS_HP_W 0x0
#define QOS_APU_R 0xF
#define QOS_APU_W 0xF


#define ARRAY_SIZE 4096
#define ITERATIONS 100
#define COMPUTE_FACTOR 200

//#define MEM_LOCATION 0x50000000U

#define TIMEDIFF(t1,t2)    (t2 - t1)
#define MILLISECONDS(t) (1000.0 * t / COUNTS_PER_SECOND)
#define EXTRACT_NIBBLE(value, index) ((Xil_In32(value) >> (index * 4)) & 0xF)

//#define ATG

XTime start, end;
double totalTime = 0.0;
XTrafGen XTrafGenInstance;

void compute_work(int factor);
void memory_stress_test(int *array, int size, int compute_factor);

void startTest();
void endTest();

int main(int argc, char *argv[]) {
    init_platform();

    sleep(5);

    Xil_DCacheDisable();
    Xil_ICacheDisable();

	configurePLQoS(QOS_HP_R, 1);
	configurePLQoS(QOS_HP_W, 2);
	configureAPUQoS(QOS_APU_R, 1);
	configureAPUQoS(QOS_APU_W, 2);

	xil_printf("APU Read QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD5C0060, 0));
	xil_printf("HP0 Read QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD380008, 0));
	xil_printf("HP1 Read QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD390008, 0));
	xil_printf("HP2 Read QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD3A0008, 0));
	xil_printf("HP3 Read QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD3B0008, 0));
	xil_printf("APU Write QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD5C0060, 4));
	xil_printf("HP0 Write QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD38001C, 0));
	xil_printf("HP1 Write QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD39001C, 0));
	xil_printf("HP2 Write QoS: 0x%X | ", EXTRACT_NIBBLE(0xFD3A001C, 0));
	xil_printf("HP3 Write QoS: 0x%X\r\n", EXTRACT_NIBBLE(0xFD3B001C, 0));

    int *array;
    int size = ARRAY_SIZE;
    int compute_factor = COMPUTE_FACTOR;

    for(int i = 0; i<10; i++){
        printf("Iteration %d: ", i);
		array = (int *)malloc(size * sizeof(int));
		if (!array) {
			fprintf(stderr, "Memory allocation failed\n");
			return -1;
		}

		for (int i = 0; i < size; ++i) {
			array[i] = rand() % 100;
		}

		startTest();
		memory_stress_test(array, size, compute_factor);
		endTest();

		free(array);
    }

    printf("%.2f\n", totalTime/10);
    printf("The application ran successfully for QOS_HP_R 0x%X, QOS_HP_W 0x%X, QOS_APU_R 0x%X, QOS_APU_W 0x%X.\n", QOS_HP_R, QOS_HP_W, QOS_APU_R, QOS_APU_W);

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
