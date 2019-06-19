//176 pixels each line, occupy 352 bytes, adopted 16bpp.
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

// DISP_MEM_OFFSET points to the starting address of the display buffer memory
#define DISP_MEM_OFFSET    0x1080000

// DISP_MEMORY_SIZE is the size of display buffer memory : SRAM
//#define DISP_MEMORY_SIZE   0x50000  /* 320K */

//Implement Write BitBLT Function
//unsigned char *VRAM = (unsigned char *)0x209e000 ;

/************************************************************
* function : MoveBitBLT
* para :   
* return : int
* recommend :
************************************************************/
int MoveBitBLT()
{

	//set interrupt control register0 , enable BitBLT interrupt.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_INTERRUPT_CONTROL0) = 0x0020;
	//enable a dummy cycle when BitBLT is either empty or full.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_CYCLE_TIMEOUT_CONTROL) = 0x0080;

	//selected 16 bpp(64K color), selected rectangular region of memory to source and destination.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_CONTROL1 ) = 0x0004; 

	//set BitBLT command register0 to Move BitBLT in positive direction with ROP
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_COMMAND0 ) = 0x0002;

	//set BitBLT ROP Code
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_COMMAND1 ) = 0x000C;

	//set BitBLT Source Start Address, the data is sourced from CPU
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_SOURCE_START_ADDRESS0 ) = 0x0000;
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_SOURCE_START_ADDRESS1 ) = 0x0000;

	//set BitBLT Destination Start Address
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_DESTINATION_START_ADDRESS0 ) = 0x1200;
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_DESTINATION_START_ADDRESS1 ) = 0x0000;

	//set BitBLT Memory Address Offset
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_MEMORY_ADDRESS_OFFSET ) = WND_WIDTH;

	//set BitBLT Width 
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_WIDTH ) = 0x0028;

	//set BitBLT Height
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_HEIGHT ) = 0x0028;

	//set BitBLT interrupt control, diable an interrupt when the BitBLT operation finished.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_INTERRUPT_CONTROL ) = 0x0000;

	//set BitBLT control register, start the 2D BitBLT operation
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_CONTROL0 ) = 0x0001; 

}

/************************************************************
* function : writeBitBLT
* para :   xleft, ytop ,xright, ybottom
* return : int
* recommend :
************************************************************/
int writeBitBLT(int xleft, int ytop, int xright, int ybottom )
{
	int iFullTag;
	int iFreeFIFOEntries;

	//set interrupt control register0 , enable BitBLT interrupt.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_INTERRUPT_CONTROL0) = 0x0020;
	//enable a dummy cycle when BitBLT is either empty or full.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_CYCLE_TIMEOUT_CONTROL) = 0x0080;

	//selected 16 bpp(64K color), selected rectangular region of memory to source and destination.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_CONTROL1 ) = 0x0004; 

	//check up the state of BitBLT FIFO
    iFullTag = IsBitBLTFIFOFull();

	if ( iFullTag )
		return 0;    //BitBLT FIFO is full  

	//get numbers of free FIFO Entries
	iFreeFIFOEntries = *(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_STATUS1 ) & 0x001f;

	//set BitBLT command register0 to write BitBLT with ROP
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_COMMAND0 ) = 0x0000;

	//set BitBLT ROP Code
#ifdef ROP2
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_COMMAND1 ) = 0x0002;
#endif
#ifdef ROP3
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_COMMAND1 ) = 0x0003;
#endif

	//set BitBLT Source Start Address, the data is sourced from CPU
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_SOURCE_START_ADDRESS0 ) = 0x0000;
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_SOURCE_START_ADDRESS1 ) = 0x0000;

	//set BitBLT Destination Start Address
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_DESTINATION_START_ADDRESS0 ) = 0x0000;
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_DESTINATION_START_ADDRESS1 ) = 0x0000;

	//set BitBLT Memory Address Offset
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_MEMORY_ADDRESS_OFFSET ) = WND_WIDTH;

	//set BitBLT Width 
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_WIDTH ) = xright - xleft;

	//set BitBLT Height
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_HEIGHT ) = ybottom - ytop;

	//set BitBLT interrupt control, diable an interrupt when the BitBLT operation finished.
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_INTERRUPT_CONTROL ) = 0x0000;

	//set BitBLT control register, start the 2D BitBLT operation
	*(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_CONTROL0 ) = 0x0001; 

}

/************************************************************
* function : IsBitBLTFIFOFull
* para :   
* return : int
* recommend : 1 -- indicate BitBLT FIFO is full;
*             0 -- indicate BitBLT FIFO is not full;
************************************************************/
int IsBitBLTFIFOFull()
{
	unsigned char uchStatus;

	uchStatus = *(volatile unsigned char *)( REGISTER_OFFSET + REG_BITBLT_STATUS0 );

	if ( uchStatus & 0x0010 == 0x0010)
		return 1;

	return 0;
}
