#include "apuqos.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define MIN_QOS_VAL 0
#define MAX_QOS_VAL 0xF

#define APU_QOS_CTRL_BASE       0xFD5C0060

const char apu_qos_name[6][9] = {
				"\0",
				"ARQoS",
				"AWQoS"
				};

static int XApuQos_wr_qos(u32 metric, u32 qos_val);
int XApuQos_SetQos(const apu_mem_info *InstancePtr, u32 qos_type, u32 Val);
int XApuQos_Init(apu_mem_info *InstancePtr);
int XApuQos_DeInit(apu_mem_info *InstancePtr);

/*
 * 	set_qos: represents set qos value :range[0-0xf]
 *
 * 	qos_type :range[1-2]
 * 	1-read qos
 * 	2-write qos
 *
 */
int configureAPUQoS(u32 qos_val, int qos_type)
{
	if (qos_val < 0 || qos_type < MIN_QOS_VAL || qos_type > MAX_QOS_VAL) {
		printf("Invalid configureAPUQoS parameters\n");
		return 0;
	}
	return XApuQos_wr_qos(qos_type, qos_val);
}


static int XApuQos_wr_qos(u32 metric, u32 qos_val) {
	apu_mem_info qos_handle;
	int ret;

	ret = XApuQos_Init(&qos_handle);
	if (ret > 0)
		return ret;

	XApuQos_SetQos(&qos_handle, metric, qos_val);

	return XApuQos_DeInit(&qos_handle);
}

int XApuQos_SetQos(const apu_mem_info *InstancePtr, u32 qos_type, u32 Val) {

	if (InstancePtr == NULL) {
		printf("%s Null instance pointer passed\n", __func__);
		return FAIL;
	}

	if (InstancePtr->IsReady != XIL_COMPONENT_IS_READY) {
		printf("Instance mapping not ready");
		return FAIL;
	}

	/*
	 * Set Qos Value
	 */
	switch(qos_type) {
		case AR_QOS:
			XApuQos_WriteReg(InstancePtr->Control_bus_BaseAddress, Val);
			break;

		case AW_QOS:
			XApuQos_WriteToHexIndex(InstancePtr->Control_bus_BaseAddress,
										 AW_QOS_OFFSET, Val);
			break;

		default:
			printf("Invalid QOS TYPE\n");
	}
	return SUCCESS;
}

int XApuQos_Init(apu_mem_info *InstancePtr) {
    if (InstancePtr == NULL) {
		printf("Instance pointer is null");
        return FAIL;
    }

    InstancePtr->Control_bus_BaseAddress = (UINTPTR) APU_QOS_CTRL_BASE;
    if (InstancePtr->Control_bus_BaseAddress == 0) {
		printf("Instance pointer base address configuration failed");
        return FAIL;
    }
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return SUCCESS;
}

int XApuQos_DeInit(apu_mem_info *InstancePtr) {
    if (InstancePtr == NULL || !InstancePtr->IsReady) {
		printf("Instance pointer is already null or not ready");
        return FAIL;
    }

    InstancePtr->Control_bus_BaseAddress = (UINTPTR) NULL;
    InstancePtr->IsReady = 0;

    return SUCCESS;
}
