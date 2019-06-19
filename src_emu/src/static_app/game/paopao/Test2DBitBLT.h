#define WND_WIDTH                                     0x0160 

#define REG_INTERRUPT_CONTROL0                        0x0A02
#define REG_CYCLE_TIMEOUT_CONTROL                     0x0A0E
#define REG_BITBLT_CONTROL0							  0x8000
#define REG_BITBLT_CONTROL1							  0x8002
#define REG_BITBLT_STATUS0								  0x8004
#define REG_BITBLT_STATUS1								  0x8006
#define REG_BITBLT_COMMAND0					          0x8008
#define REG_BITBLT_COMMAND1							  0x800A
#define REG_BITBLT_SOURCE_START_ADDRESS0				  0x800C
#define REG_BITBLT_SOURCE_START_ADDRESS1                 0x800E
#define REG_BITBLT_DESTINATION_START_ADDRESS0            0x8010
#define REG_BITBLT_DESTINATION_START_ADDRESS1            0x8012
#define REG_BITBLT_MEMORY_ADDRESS_OFFSET				  0x8014
#define REG_BITBLT_WIDTH								  0x8018
#define REG_BITBLT_HEIGHT                               0x801C
#define REG_BITBLT_BACKGROUND_COLOR                           0x8020
#define REG_BITBLT_FOREGROUND_COLOR                           0x8024
#define REG_BITBLT_INTERRUPT_STATUS                      0x8030
#define REG_BITBLT_INTERRUPT_CONTROL                     0x8032
#define REG_BITBLT_DATAMEMO_MAPPED_REGION                0x10000


// REGISTER_OFFSET points to the starting address of the S1D13712 registers
#define REGISTER_OFFSET    0x1000000      // 0x1000000-0x107ffff


/**********************************************************
*
*       function prototype
*
***********************************************************/
int MoveBitBLT();
