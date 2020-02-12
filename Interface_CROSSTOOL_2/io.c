/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/
#include "io.h"
#include "printf.h"

void	setLowVector(void);
void	writeVectorBaseRegister(int);
void	reverseLED(void);
void	noOperation (void);

void	putChar1(int);
void	putChar(int);

#define	BUFSIZE	128
char txbuf[BUFSIZE];
void enQue(int);
int deQue();
char *volatile inp=txbuf;
char *volatile outp=txbuf;

int	getChar(void) {
	struct rpiUart * rpiUart = RPI_UART_BASE;
	while (rpiUart->FR & 0x10);
	return rpiUart->DR;
}

void	putChar0(int c ) {
	struct rpiUart * rpiUart = RPI_UART_BASE;
	if(c == '\n') putChar0('\r');
	while (!(rpiUart->FR & 0x80));
	rpiUart->DR = c;
}

void initializeJtagPin(){
	volatile static int wait=0;
	struct rpiGpio *rpiGpio = RPI_GPIO_BASE;

	// ALT4=3(011) ALT5=2(010)                                 P1 JTAG
	rpiGpio->GPFSEL2 |= (3 << (22-20)*3);	//06 GPIO22 (ALT4) 15-03TRST
	rpiGpio->GPFSEL0 |= (2 << (04-00)*3);	//12 GPIO04 (ALT5) 07-05TDI
	rpiGpio->GPFSEL2 |= (3 << (27-20)*3);	//21 GPIO27 (ALT4) 13-07TMS
	rpiGpio->GPFSEL2 |= (3 << (25-20)*3);	//15 GPIO25 (ALT4) 22-09TCK
	rpiGpio->GPFSEL2 |= (3 << (24-20)*3);	//12 GPIO24 (ALT4) 18-13TDO

	while (wait) {
		// Wait until the variable wait will changed by GDB.
	}
}

void initializeCPU() {
}

void initializeI2C() {
}

void initializeSPI() {
}

void initializeBoardLED() {
	struct rpiGpio *rpiGpio = RPI_GPIO_BASE;

	/* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
	peripheral register to enable GPIO16 as an output */
	rpiGpio->GPFSEL1 |= (1 << (16-10)*3);	//18 GPIO16
	rpiGpio->GPFSEL4 |= (1 << (47-40)*3);	//21 GPIO47
}

void initializeGPIO() {
	struct rpiGpio *rpiGpio = RPI_GPIO_BASE;

	//	R1,G1,B1	//	R2,G2,B2
	//	A ,B ,C		//	CLK,LATCH,OE

	//	GPIO 05,06,//07	//	GPIO 12,13
	//	GPIO 16,17,18	//	GPIO 19,20,21
	//	GPIO 23,26

	//	GPIO 05,06
	rpiGpio->GPFSEL0 |= (1 << ( 5- 0)*3);	//15 GPIO05
	rpiGpio->GPFSEL0 |= (1 << ( 6- 0)*3);	//18 GPIO06
	rpiGpio->GPFSEL0 |= (1 << ( 7- 0)*3);	//21 GPIO07 ->CE1
	//	GPIO 12,13
	rpiGpio->GPFSEL1 |= (1 << (12-10)*3);	//06 GPIO12
	rpiGpio->GPFSEL1 |= (1 << (13-10)*3);	//09 GPIO13

	//	GPIO 16,17,18
	rpiGpio->GPFSEL1 |= (1 << (16-10)*3);	//18 GPIO16
	rpiGpio->GPFSEL1 |= (1 << (17-10)*3);	//21 GPIO17
	rpiGpio->GPFSEL1 |= (1 << (18-10)*3);	//24 GPIO18

	//	GPIO 19,20,21
	rpiGpio->GPFSEL1 |= (1 << (19-10)*3);	//27 GPIO19
	rpiGpio->GPFSEL2 |= (1 << (20-20)*3);	//00 GPIO20
	rpiGpio->GPFSEL2 |= (1 << (21-20)*3);	//03 GPIO21

	//	GPIO 23,26
	rpiGpio->GPFSEL2 |= (1 << (23-20)*3);	//09 GPIO23
	rpiGpio->GPFSEL2 |= (1 << (26-20)*3);	//18 GPIO26
}

void initializeUART0() {
	struct rpiUart *rpiUart = RPI_UART_BASE;
	struct rpiGpio *rpiGpio = RPI_GPIO_BASE;
	struct rpiIrqController *rpiIRQController =
		(struct rpiIrqController *)RPI_INTERRUPT_CONTROLLER_BASE;

        rpiUart->CR = 0;
        //GPIO UART Set
/**/
	rpiGpio->GPPUD = 0x01; //Pull Down
	{int i;for(i=0;i<150;i++){ noOperation(); }}
	rpiGpio->GPPUDCLK0 =  (0x01 << 14) | (0x01 <<15);  //Clock on
	{int i;for(i=0;i<150;i++){ noOperation(); }}
	rpiGpio->GPPUDCLK0 =  0x00;  //Clock off
	rpiGpio->GPPUD = 0x00; //Disable
/**/

// ALT0=4(100) ALT1=5(101) ALT2=6(110) ALT3=7(111) ALT4=3(011) ALT5=2(010)
	rpiGpio->GPFSEL1 &= ~(7 << (14-10)*3);	//12 GPIO14
	rpiGpio->GPFSEL1 |= (4 << (14-10)*3);	//12 GPIO14
	rpiGpio->GPFSEL1 &= ~(7 << (15-10)*3);	//15 GPIO15
	rpiGpio->GPFSEL1 |= (4 << (15-10)*3);	//15 GPIO15

        // Baurd rate Set
        //(3000000 / (16 * 115200) = 1.627	//(0.627*64)+0.5 = 40
        rpiUart->IBRD = 1 ;
        rpiUart->FBRD = 40;

        // stick parity dis, 8bit, FIFO en, two stop bit no, odd parity, parity dis, break no
       // rpiUart->LCRH = 0x70;

        rpiUart->LCRH = 0x60; //FIFO Disable

        rpiUart->IFLS = 0x0000;	//FIFO Interrupt Level
        //rpiUart->IMSC = 0x030;	// RXIM(4)
        rpiUart->IMSC = 0x030;	// RXIM(4)
        //rpiUart->IMSC = 0x000;	// RXIM(4)
	rpiIRQController->Enable_IRQs_2 = 0x01<<25;
/*
        rpiUart->IMSC = 0x030;	// TXIM(5) RXIM(4)
        rpiUart->RIS = 0x0301;	// Raw Interrupt
        rpiUart->MIS = 0x0301;	// Masked  Interrupt
        rpiUart->ICR = 0x0301;	// Interrupt Clear Register
*/

        // CR
// CTS dis, RTS dis, OUT1-2=0, RTS dis, DTR dis, RXE en, TXE en, loop back dis, SIRLP=0, SIREN=0, UARTEN en
        rpiUart->CR = 0x0301;

}

void initializeTimer() {
	struct rpiGpio *rpiGpio = (struct rpiGpio*)RPI_GPIO_BASE;
	struct rpiArmTimer *rpiArmTimer = (struct rpiArmTimer *)RPI_ARMTIMER_BASE;
	struct rpiIrqController *rpiIRQController =
		(struct rpiIrqController *)RPI_INTERRUPT_CONTROLLER_BASE;

	/* Enable the timer interrupt IRQ */
	rpiIRQController->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

	/* Setup the system timer interrupt */
	/* Timer frequency = Clk/256 * 0x400 */
	////rpiArmTimer->Load = 0x400;
	////rpiArmTimer->Load = (0x400/50);     //
	////rpiArmTimer->Load = (0x400/128);     //
	//rpiArmTimer->Load = 1;     //
	//rpiArmTimer->Load = 0x400;     //
        /* Setup the system timer interrupt */
        rpiArmTimer->PreDivider = 0xF9;     //(=1/250)  1MH 0x7D(126)

        /* Timer frequency = Clk(1MHz)/1000  */
        //rpiArmTimer->Load = 10000 -1;     //
        rpiArmTimer->Load = 50-1 ;     //
        //rpiArmTimer->Reload = 1000-1 ;     //


	/* Setup the ARM Timer */
	rpiArmTimer->Control =
		RPI_ARMTIMER_CTRL_23BIT |
		RPI_ARMTIMER_CTRL_ENABLE |
		RPI_ARMTIMER_CTRL_INT_ENABLE |
		RPI_ARMTIMER_CTRL_PRESCALE_256;
//
//		250MHz	PreDevider 0x7D(126)? = ~2MHz (Maby 2MHz)
//		1024/50=20msec? 1024=1msec??
//

}

/////////////////////////////////////////////////////////////////////

extern volatile int timer;

void uartHandler0()
{
	struct rpiUart * rpiUart = (struct rpiUart *)RPI_UART_BASE;

        if(rpiUart->MIS & 0x0010) {	// RXIM(4)
		putChar0(getChar());
		rpiUart->ICR = 0x0010;	// Interrupt Clear Register
	}
        if(rpiUart->MIS & 0x0020) {	// TXIM(5)
        	rpiUart->IMSC &= ~0x020;
		//putchar();
		rpiUart->ICR = 0x0020;	// Interrupt Clear Register
	}
/*
        rpiUart->IFLS = 0x0000;	//FIFO Interrupt Level
        rpiUart->IMSC = 0x030;	// TXIM(5) RXIM(4)
        rpiUart->RIS = 0x0301;	// Raw Interrupt
        rpiUart->MIS = 0x0301;	// Masked  Interrupt
        rpiUart->ICR = 0x0301;	// Interrupt Clear Register
*/
}

void uartHandler1()
{
	int	data;
	int	irq;
	struct rpiAux* rpiAux= RPI_AUX_BASE;

	if(!(rpiAux->IRQ & 0x01)) {
		putChar1('?');	// not mini UART interrupt ?
		putChar1('1');	// not mini UART interrupt ?
	}

	//if(rpiAux->MU_LSR_REG & 0x01) {

	irq = rpiAux->MU_IIR_REG & 0x06 ;

	if( irq== 0x04) {
		putChar(data =rpiAux->MU_IO_REG) ;
		//rpiAux->MU_IO_REG=data;
	} else if ( irq == 0x02) {
		if((data = deQue())  < 0) {	// Disable Transmit Interrupt
			rpiAux->MU_IER_REG=0x01;	//Enable Receive Only(1)
		//	putChar1('&');	// not mini UART interrupt ?
		} else {
			//if(rpiAux->MU_LSR_REG &0x20){
			//if(rpiAux->MU_IIR_REG & 0x02) {
			rpiAux->MU_IO_REG=data;
			rpiAux->MU_IER_REG=0x03;	//Enable R(1)&T(2)
		//	putChar1('?');	// not mini UART interrupt ?
		}
	} else {
		putChar1('?');	// not mini UART interrupt ?
		putChar1('2');	// not mini UART interrupt ?
	}
}

void	imatrix(void);
void timerHandler()
{
	static int time = 0;
	struct rpiArmTimer * rpiArmTimer = (struct rpiArmTimer *)RPI_ARMTIMER_BASE;

	rpiArmTimer->IRQClear = 1;
	timer ++;
//	imatrix();
	if(time++ <50) {
		return;
	}
	time = 0;
	reverseLED();

}

void reverseLED() {	//Only for Raspberry Pi1 and Pi2  not for Pi3
	struct rpiGpio * rpiGpio = (struct rpiGpio*)RPI_GPIO_BASE;

//	if( rpiGpio->GPLEV0 & (1 << 16) ) {	// Raspberry PI B
//		rpiGpio->GPCLR0 = (1 << 16);
//	} else {
//		rpiGpio->GPSET0 = (1 << 16);
//	}

	if( rpiGpio->GPLEV1 & (1 << 15) ) {	// Raspberry PI B+ /PI2 B
		rpiGpio->GPCLR1 = (1 << 15);
	} else {
		rpiGpio->GPSET1 = (1 << 15);
	}

}

unsigned int uart_lcr ( void )
{
	struct rpiAux* rpiAux= RPI_AUX_BASE;
	return(rpiAux->MU_LSR_REG);
}

int getChar1 ( void )
{
	struct rpiAux* rpiAux= RPI_AUX_BASE;

	while(!(rpiAux->MU_LSR_REG & 0x01)) {
	}
	return(rpiAux->MU_IO_REG & 0xFF);
}
//------------------------------------------------------------------------
unsigned int uart1Check ( void )
{
	struct rpiAux* rpiAux= RPI_AUX_BASE;

	if(rpiAux->MU_LSR_REG & 0x01) return(1);
	else	return(0);

}
//------------------------------------------------------------------------
void putChar1 (int c )
{
	struct rpiAux* rpiAux= RPI_AUX_BASE;

	if(c == '\n') putChar1('\r');
	while(!(rpiAux->MU_LSR_REG &0x20)){
	}
	rpiAux->MU_IO_REG=c;
}
//------------------------------------------------------------------------
void uart1Flush ( void )
{
	struct rpiAux* rpiAux= RPI_AUX_BASE;

	while(rpiAux->MU_LSR_REG & 0x100) {
	}
}

void initializeUART1( void )
{
	struct rpiAux* rpiAux= RPI_AUX_BASE;
	struct rpiGpio *rpiGpio = RPI_GPIO_BASE;
	struct rpiIrqController *rpiIRQController =
		(struct rpiIrqController *)RPI_INTERRUPT_CONTROLLER_BASE;
	unsigned int ra;

	rpiAux->ENABLES=1;	//enable miniUART
	rpiAux->MU_IER_REG=0;	//disable interrupt
	rpiAux->MU_CNTL_REG=0;	//01 Receive Enable 03 Transmit Enalbe
	rpiAux->MU_LCR_REG=3;	//0x 8bit mode	3?	DLAB access = 0x80
	rpiAux->MU_MCR_REG=0;	//Modem Control 0
	rpiAux->MU_IER_REG=0;
	rpiAux->MU_IIR_REG=0xC6;	// 0xC6 FIFO Enable/Clear
		//((250,000,000/115200)/8)-1 = 270
	rpiAux->MU_BAUD_REG=270;

        rpiGpio->GPFSEL1 &= ~(7 << (14-10)*3);   //12 GPIO14
        rpiGpio->GPFSEL1 |= (2 << (14-10)*3);   //12 GPIO14
        rpiGpio->GPFSEL1 &= ~(7 << (15-10)*3);   //15 GPIO15
        rpiGpio->GPFSEL1 |= (2 << (15-10)*3);   //15 GPIO15

	rpiGpio->GPPUD = 0;
	for(ra=0;ra<150;ra++) noOperation();
	rpiGpio->GPPUDCLK0 = ((1<<14)|(1<<15));
	for(ra=0;ra<150;ra++) noOperation();
	rpiGpio->GPPUDCLK0 = 0;
	rpiAux->MU_CNTL_REG=3; //01 Receive Enable 03 Transmit Enalbe


	rpiIRQController->Enable_IRQs_1 = 0x01<<29;
	rpiAux->MU_IER_REG=0x03;	//Enable Receive Interrupt 0x03
				//Enable Transmit Interrupt 0x01

        inp=txbuf;
        outp=txbuf;
}


void initializeUART() {
#ifdef	AARCH64
	initializeUART1();
#else
	initializeUART0();
#endif
}


#ifdef	AARCH64

void	putChar(int data)
{
	if(data =='\n') {
		putChar('\r');
	}

	enQue(data);
}
#else

void	putChar(int data)
{
	putChar0(data);
}
#endif

void enQue(int data){
	struct rpiIrqController *rpiIRQController =
		(struct rpiIrqController *)RPI_INTERRUPT_CONTROLLER_BASE;
	struct rpiAux* rpiAux= RPI_AUX_BASE;
	char	*temp =inp;
	if(++temp >= txbuf+sizeof(txbuf)) {
		temp= txbuf;
	}

//	if(temp ==outp) {
	while(temp == outp) {
		rpiAux->MU_IER_REG=0x03;	//Enable Receive Interrupt 0x03
//		temp =inp;
//		if(++temp >= txbuf+sizeof(txbuf)) {
//			temp= txbuf;
//		}
//		return;
	}
	*inp = data;
	inp = temp;

	rpiAux->MU_IER_REG=0x03;	//Enable Receive Interrupt 0x03
}

int	deQue() {
	int data;

	if(outp == inp) {
		data =  -1;
	} else {
		data = *outp;
		if(++outp >= txbuf+sizeof(txbuf)) {
			outp= txbuf;
		}
	}
	return data;

}

int	adc() {

//	int	d0,ad1;
//	ad0 = getADC3008(0);
//	ad1 = getADC3008(1);
//	printf("\nAD:%d %d\n",ad0,ad1);

}

int	gpio() {

//	matrix();

//	setMCP23017(0x20,0x0A,0x00);
//	setMCP23017(0x20,0x01,0x00);
//	setMCP23017(0x20,0x13,0x5a);
//	setMCP23017(0x20,0x13,0xFF^ getMCP23017(0x20,0x13)    );
}

void	noOperation()
{
}
