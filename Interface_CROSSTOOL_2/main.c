/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/

#include "printf.h"
#include "video2.h"

void	disableInterrupt(void);
void	enableInterrupt(void);

int	getChar(void),getChar0,getChar1();
void	putChar(int ),putChar0(),putChar1();
void	pie(void),pi(),e(),adc(),gpio();

void	initializeARM(void) ;
void	initializeHardware();
void	initializeCPU(void);
void	initializeBoardLED(void);
void	initializeUART(void);
void	initializeTimer(void);
void	initializeInterrupt(void);
void	initializeJtagPin(void);

void	initializeI2C(void) ;
void	initializeSPI(void) ;
void	initializeGPIO(void) ;

volatile	int	timer;

static void stdout_putf(void *unused, char c)
{
	putChar(c);
}


int	main( )
{
	fb_info_t fb_info = {1280, 800, 1280, 800, 0, 16, 0, 0, 0, 0};

	initializeHardware();

	init_printf(0, stdout_putf);

	lfb_init(&fb_info);

	unsigned char *vram  = fb_info.buf_addr;
	unsigned int  pitch  = fb_info.row_bytes;
	unsigned int  bpp    = fb_info.bpp;
	unsigned int  x = fb_info.display_w;
	unsigned int  y = fb_info.display_h;

	boxfill8(vram, COL8_008484, pitch, bpp,  0,     0,      x -  1, y - 29);
	boxfill8(vram, COL8_C6C6C6, pitch, bpp,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, COL8_FFFFFF, pitch, bpp,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, COL8_C6C6C6, pitch, bpp,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, COL8_FFFFFF, pitch, bpp,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, COL8_FFFFFF, pitch, bpp,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, COL8_848484, pitch, bpp,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, COL8_848484, pitch, bpp, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, COL8_000000, pitch, bpp,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, COL8_000000, pitch, bpp, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, COL8_848484, pitch, bpp, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, COL8_848484, pitch, bpp, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, COL8_FFFFFF, pitch, bpp, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, COL8_FFFFFF, pitch, bpp, x -  3, y - 24, x -  3, y -  3);

	printf("\nWelcome to ARM Common Software (%s %s).\n", __DATE__ , __TIME__);

	while(1) {
		static int	n=0;
		pi();
		e();
		printf("\n%8d\ttime=\t%16d\n",++n,timer);
	}
}

void initializeHardware() {

	disableInterrupt();

	initializeARM();

	initializeJtagPin();	// Only for Raspberry PI  move to crt0.s

	initializeCPU();

	initializeBoardLED();

	initializeUART();	// initialize RS232C serial port (115200bps)

	initializeI2C();	// initialize I2C serial port

	initializeSPI();	// initialize SPI serial port

	initializeGPIO();	// initialize GPIO serial port

	initializeTimer();	// 	set OS Timer

	enableInterrupt();

//	fb_init();
}

//***********************************************************************/
//*	GNU Linker .text/.bss Initialize routine before main function.	*/
//***********************************************************************/

extern char etext[], sdata[], edata[], sbss[] , ebss[];

void initializeMemory()
{
	char *src, *dst;

	for( src = etext, dst = sdata ; dst< edata ; )
		*dst++ = *src++;

	for( dst=sbss                 ; dst< ebss  ; )
		*dst++ =0;

}


//***********************************************************************/
//*	ARM Architecture Dependent Code					*/
//***********************************************************************/


//***********************************************************************/
//*	ARMv4T / ARMv5T / ARMv6 / ARMv7-A (Original Basic Architecture)	*/
//*	32bit ARM	ARM7 / ARM9 / ARM11 / Cortex-A7/A8/A9 seriese	*/
//***********************************************************************/

void	setLowVector(void);		// setHighVector();
void	disableDataCache(void);
void	disableInstructionCache(void);



extern void (instructionTable)(void);	/* Address 0x0000 0000 */
void	writeVectorBaseRegister(void(*)(void));

void initializeARM() {

	setLowVector();		// setHighVector();
	writeVectorBaseRegister(instructionTable);

	disableDataCache();
	disableInstructionCache();

}
