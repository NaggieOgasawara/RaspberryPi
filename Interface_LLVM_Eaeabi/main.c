/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/

#include "printf.h"

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

	initializeHardware();

	init_printf(0, stdout_putf);

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
