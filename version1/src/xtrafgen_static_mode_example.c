/******************************************************************************
* Copyright (C) 2013 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xtrafgen_static_mode_example.c
 *
 * This file demonstrates how to use the xtrafgen driver on the Xilinx AXI
 * Traffic Generator core. The AXI Traffic Generator IP is designed to
 * generate AXI4 traffic which can be used to stress different modules/
 * interconnect connected in the system.
 *
 * This example demonstrates  how to use the Static mode in the Axi Traffic
 * Genrator.In Static mode the core continuously generates fixed address and
 * fixed INCR type read and write transfers based on the burst length
 * configured.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.01a adk  03/09/13 First release
 * 2.00a adk  16/09/13 Fixed CR:737291
 * 4.1   ms   01/23/17 Modified xil_printf statement in main function to
 *                     ensure that "Successfully ran" and "Failed" strings
 *                     are available in all examples. This is a fix for
 *                     CR-965028.
 *       ms   04/05/17 Added tabspace for return statements in functions for
 *                     proper documentation while generating doxygen.
 * </pre>
 *
 * ***************************************************************************
 */

/***************************** Include Files *********************************/
#include "xtrafgen.h"
#include "xparameters.h"
#include "xil_exception.h"

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#define TRAFGEN_DEV_ID	XPAR_XTRAFGEN_0_DEVICE_ID

#define BURST_LEN	255

#define MEM_ADDR	0xB0010000U

#define DATA 		0xc0015afe

#undef DEBUG

int InitializeXTrafGenStaticMode(XTrafGen *InstancePtr)
{

	XTrafGen_Config *Config;
	int Status = XST_SUCCESS;
	u32 Count;
	Count = 0;

	/* Initialize the Device Configuration Interface driver */
	Config = XTrafGen_LookupConfig(TRAFGEN_DEV_ID);

	if (!Config) {
		xil_printf("No config found for %d\r\n", TRAFGEN_DEV_ID);
		return XST_FAILURE;
	}

	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XTrafGen_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed\n\r");
		return Status;
	}

	/* Check for the Static Mode */
	if(InstancePtr->OperatingMode != XTG_MODE_STATIC) {
		return XST_FAILURE;
	}

	/* Fill the BRAM with the data */
	for (Count = 0; Count < (5 * BURST_LEN); Count++) {
		*((u32 *)(MEM_ADDR + Count*4)) = 0;
		if(*((u32 *)(MEM_ADDR + Count*4)) != 0x00) {
			xil_printf("ERROR: Memory Addr 0x%x = 0x%x \t"
					"Expected = 0x00\n\r"
					,(MEM_ADDR + Count*4),*((u32 *)(MEM_ADDR + Count*4)));
			return XST_FAILURE;
		}
	}

	/* Set the Required Burst length */
	XTrafGen_SetStaticBurstLen(InstancePtr, BURST_LEN);

	/* Disable the traffic generation */
	XTrafGen_StaticDisable(InstancePtr);

	/* Check for the Static control DONE Bit resets */
	XTrafGen_SetStaticTransferDone(InstancePtr);
	Status = XTrafGen_IsStaticTransferDone(InstancePtr);
	if (Status != TRUE) {
		xil_printf("Static control reset failed failed\n\r");
		return Status;
	}

	return XST_SUCCESS;
}

int EnableXTrafGenStaticMode(XTrafGen *InstancePtr) {


	/* Enable the traffic generation */
	XTrafGen_StaticEnable(InstancePtr);

	return XST_SUCCESS;
}

int DisableXTrafGenStaticMode(XTrafGen *InstancePtr) {
	int Status = XST_SUCCESS;
	u32 Count;
	Count = 0;

	/* Disable the traffic generation */
	XTrafGen_StaticDisable(InstancePtr);

	/* Wait till data transmission completes */
	do {
		Status = XTrafGen_GetStaticTransferDone(InstancePtr);
	}while(Status != XTG_STREAM_CNTL_TD_MASK);


	/* Check for the data
	 * Note:" 0xc0015afe " is the fixed data that has been given by the core
	 * continuously when ATG is configured in STATIC mode and RUN.
	 * There is no way to change this DATA in SW.
	 */
	for (Count = 0; Count < (BURST_LEN + 1); Count++) {
		if(*((u32 *)(MEM_ADDR + Count*4)) != DATA) {
			xil_printf("ERROR: Memory Addr 0x%x = 0x%x \t"
					"Expected = 0xc0015afe\n\r"
					,(MEM_ADDR + Count*4),*((u32 *)(MEM_ADDR + Count*4)));
			return XST_FAILURE;
		}
	}

	for (Count = (BURST_LEN+1); Count < (BURST_LEN + 4); Count++) {
			if(*((u32 *)(MEM_ADDR + Count*4)) == DATA) {
				xil_printf("ERROR: Memory Addr 0x%x = 0x%x \t"
						"Something else expected\n\r"
						,(MEM_ADDR + Count*4),*((u32 *)(MEM_ADDR + Count*4)));
				return XST_FAILURE;
			}
		}

	return XST_SUCCESS;
}
