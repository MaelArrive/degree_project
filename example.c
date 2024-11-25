 

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "PMU.h"
#include "xtime_l.h"
#include "xil_mmu.h"
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include <unistd.h>

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

 
 
#include "xil_io.h"
#include "xpseudo_asm.h"
#include "xparameters.h"
#include "xreg_cortexa53.h"
#include "xil_exception.h"
#include "bspconfig.h"

int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

static XTime XMt_CalcTime(XTime tCur) {
	XTime tEnd;
	XTime tDiff;
	XTime_GetTime(&tEnd);
	tDiff = tEnd - tCur;
	return tDiff;
}

static void enable_cycle_counter_el0(void) {
	uint64_t val;
	/* Disable cycle counter overflow interrupt */
	asm volatile("msr pmintenset_el1, %0" : : "r" ((uint64_t)(0 << 31)));
	/* Enable cycle counter */
	asm volatile("msr pmcntenset_el0, %0" :: "r" (1 << 31));
	/* Enable user-mode access to cycle counters. */
	asm volatile("msr pmuserenr_el0, %0" :: "r" ((1 << 0) | (1 << 2)));
	/* Clear cycle counter and start */
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	val |= ((1 << 0) | (1 << 2));
	asm volatile("isb");
	asm volatile("msr pmcr_el0, %0" :: "r" (val));
	val = (1 << 27);
	asm volatile("msr pmccfiltr_el0, %0" :: "r" (val));
}

static inline uint64_t read_pmccntr(void) {
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
	return val;
}




void snoop_disable () {

	u64 val=0;

	val= mfcp(S3_1_c15_c2_1 );

	val =0x00;

	mtcp(S3_1_C15_C2_1,val);

	val= mfcp(S3_1_c15_c2_1 );
	val=val+0;

}
void mmu () {

	u64 val=0;

	val= mfcp( TCR_EL3 );

	mtcp( TCR_EL3,val);

	val= mfcp(S3_1_c15_c2_1 );
	val=val+0;

}


#define  size   140000

int  *m=  0x10000000;
//long int   tab[10];
int main()
{

	init_platform();

	void XTime_StartTimer(void);
	XTime tCur1;
	XTime_GetTime(&tCur1);
	srand( tCur1);
	int kk=0;
	register int a;
	//snoop_disable();


	//Xil_ConfigureL1Prefetch(0);

	while(1) {

		XTime_GetTime(&tCur1);
		enable_cycle_counter_el0();


		select_evt_counter(1)   ;
		event_track(0xc8);
		enable_evt_counter(1);
		reset_evt_counter(0);
		select_evt_counter(1);

		reset_all_counters(0);
		u64 begin2=read_evt_counter();
		enable_all_counters(0x3F);

		u64 begin= (u64) read_pmccntr();
		for ( int j=0; j < size; j=j+1){

				*(m+j)=*(m+j-1)+*(m+j+1)+*(m+j);

				 //*(m+j)=j;
				 //kk=rand()%size;
				 //*(m+kk)=j;
			//	*(m+j+1)=*(m+j);
			}

		u64 end= (u64) read_pmccntr();

		 disable_all_counters(0x3F);
		 select_evt_counter(1);
		 u64 end2 =  (u64)  read_evt_counter();



		XTime endTime = XMt_CalcTime(tCur1);
		a++;
		//tab[kk%10]= end-begin;
		//kk++;
		printf(" %lu  %lu %d  \n\r", end-begin, end2-begin2 ,a);
	}
	cleanup_platform();
	return 0;
}


 
 
 
