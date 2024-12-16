/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "axiqos.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define MIN_QOS_VAL 0
#define MAX_QOS_VAL 0xF

#define AXI_QOS_CTRL_BASE       0xFD360000

const char axi_port_name[7][5] = {
				"HPC0",
				"HPC1",
				"HP0",
				"HP1",
				"HP2",
				"HP3"
				};

const char axi_qos_name[6][9] = {
				"\0",
				"RDQoS",
				"WRQoS",
				"RDISSUE",
				"WRISSUE",
				};

static int XAxiQos_wr_qos(u32 metric, u32 port_num, u32 qos_val);
int XAxiQos_SetQos(const axi_mem_info *InstancePtr, u32 qos_type, u32 Val);
int XAxiQos_Init(axi_mem_info *InstancePtr, unsigned int port_num);
int XAxiQos_DeInit(axi_mem_info *InstancePtr);

/*
 * 	set_qos: represents set qos value :range[0-0xf]
 *
 * 	qos_type :range[1-4]
 * 	1-read qos
 * 	2-write qos
 * 	3-outstanding_rdissue
 * 	4-outstanding_wrissue
 *
 */
int configurePLQoS(u32 qos_val, int qos_type)
{
	if (qos_val < 0 || qos_type < MIN_QOS_VAL || qos_type > MAX_QOS_VAL) {
		printf("Invalid configurePLQoS parameters\n");
		return 0;
	}

	for (u32 port = 2; port < 6; port++) {
		int status = XAxiQos_wr_qos(qos_type, port, qos_val);
		if(status != SUCCESS) return FAIL;
	}

	return SUCCESS;
}


static int XAxiQos_wr_qos(u32 metric, u32 port_num, u32 qos_val) {
	axi_mem_info qos_handle;
	int ret;

	ret = XAxiQos_Init(&qos_handle, port_num);
	if (ret > 0)
		return ret;

	XAxiQos_SetQos(&qos_handle, metric, qos_val);

	return XAxiQos_DeInit(&qos_handle);
}

int XAxiQos_SetQos(const axi_mem_info *InstancePtr, u32 qos_type, u32 Val) {

	if (InstancePtr == NULL) {
		printf("%s Null instance pointer passed\n", __func__);
		return FAIL;
	}

	if (InstancePtr->IsReady != XIL_COMPONENT_IS_READY) {
		printf("Instance mapping not ready");
		return FAIL;
	}

	if (Val < MIN_QOS_VAL || Val > MAX_QOS_VAL)
		return FAIL;
	/*
	 * Set Qos Value
	 */
	switch(qos_type) {
		case RD_QOS:
			XAxiQos_WriteReg(InstancePtr->Control_bus_BaseAddress,
										 HPPORT_RD_QOS_OFFSET, Val);
			break;

		case WR_QOS:
			XAxiQos_WriteReg(InstancePtr->Control_bus_BaseAddress,
										 HPPORT_WR_QOS_OFFSET, Val);
			break;

		case RD_ISSUE:
			XAxiQos_WriteReg(InstancePtr->Control_bus_BaseAddress,
										 HPPORT_RD_ISSUE_OFFSET, Val);
			break;

		case WR_ISSUE:
			XAxiQos_WriteReg(InstancePtr->Control_bus_BaseAddress,
										 HPPORT_WR_ISSUE_OFFSET, Val);
			break;
		default:
			printf("Invalid QOS TYPE\n");
	}
	return SUCCESS;
}

int XAxiQos_Init(axi_mem_info *InstancePtr, unsigned int port_num) {
    if (InstancePtr == NULL) {
		printf("Instance pointer is null");
        return FAIL;
    }

    InstancePtr->Control_bus_BaseAddress = (UINTPTR) AXI_QOS_CTRL_BASE + port_num * 0x10000;
    if (InstancePtr->Control_bus_BaseAddress == 0) {
		printf("Instance pointer base address configuration failed");
        return FAIL;
    }
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return SUCCESS;
}

int XAxiQos_DeInit(axi_mem_info *InstancePtr) {
    if (InstancePtr == NULL || !InstancePtr->IsReady) {
		printf("Instance pointer is already null or not ready");
        return FAIL;
    }

    InstancePtr->Control_bus_BaseAddress = (UINTPTR) NULL;
    InstancePtr->IsReady = 0;

    return SUCCESS;
}
