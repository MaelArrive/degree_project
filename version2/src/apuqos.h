#ifndef APU_QOS_H_
#define APU_QOS_H_

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include "xil_types.h"

#define QOS_INSTANCE_NAME	"axi_qos"
#define XApuQos_ReadReg(addr,off) 					*((volatile uint32_t *) ( ((uint8_t*)addr) + off ))
#define XApuQos_WriteReg(addr, val)			*((volatile uint32_t *) ( ((uint8_t*)addr) )) = (val)
#define XApuQos_WriteToHexIndex(addr, off, val) 	*((volatile uint32_t *) ( ((uint8_t*)addr) )) = (*((volatile uint32_t *) ((uint8_t*)addr)) & ~(0xF << (off * 4))) | ((val & 0xF) << (off * 4))

#define APU_QOS_CTRL_BASE       0xFD5C0060

#define SUCCESS		0
#define FAIL		-1

/************************** Constant Definitions ****************************/
#define AW_QOS_OFFSET        4

/**************************** Type Definitions ******************************/
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short int u16;

typedef struct {
    int isInitialized;
    int IsReady;
    UINTPTR Control_bus_BaseAddress;
} apu_mem_info;

enum ApuQos_Type {
	AR_QOS = 1,
	AW_QOS
};

int configureAPUQoS(u32 qos_val, int qos_type);

#endif /* UAFIFM_QOS_H_ */
