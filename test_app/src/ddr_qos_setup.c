#include <stdint.h>
#include <stdio.h>

#define PORT_TYPE_CTRL 0xFD090000U
#define PORT5_TYPE_MASK 0x0000C000U
#define PORT4_TYPE_MASK 0x00003000U
#define PORT3_TYPE_MASK 0x00000C00U
#define PORT2_TYPE_MASK 0x000003C0U
#define PORT1_TYPE_MASK 0x0000003CU
#define PORT0_TYPE_MASK 0x00000003U

#define QOS_CTRL 0xFD090004U
#define PORT5_THROTTLE_MASK 0x00780000U
#define PORT4_THROTTLE_MASK 0x00070000U
#define PORT3_THROTTLE_MASK 0x0000E000U
#define PORT2_THROTTLE_MASK 0x00001F00U
#define PORT1_THROTTLE_MASK 0x000000F8U
#define PORT0_THROTTLE_MASK 0x00000007U

int write_to_address(uint32_t address, uint32_t mask, uint32_t value) {
    volatile uint32_t *ptr = (volatile uint32_t *)address;
    uint32_t current_value = *ptr;
    uint32_t new_value = (current_value & ~mask) | (value & mask);

    *ptr = new_value;

    if (*ptr != new_value) {
    	printf("write operation failed");
		return -1;
	}

    return 0;
}

void ddr_qos_setup()
{
	write_to_address(PORT_TYPE_CTRL, PORT1_TYPE_MASK,0x00000028U);
	write_to_address(PORT_TYPE_CTRL, PORT3_TYPE_MASK,0x00000400U);
	write_to_address(PORT_TYPE_CTRL, PORT4_TYPE_MASK,0x00001000U);
	write_to_address(PORT_TYPE_CTRL, PORT5_TYPE_MASK,0x00004000U);

	write_to_address(QOS_CTRL, PORT3_THROTTLE_MASK, 0x0000E000U);
	write_to_address(QOS_CTRL, PORT4_THROTTLE_MASK, 0x00070000U);
	write_to_address(QOS_CTRL, PORT5_THROTTLE_MASK, 0x00780000U);
}
