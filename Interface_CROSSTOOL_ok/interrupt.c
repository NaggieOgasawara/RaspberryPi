/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/
#include	"io.h"
#include	"printf.h"
void uartHandler0();
void uartHandler1();
void timerHandler();

extern volatile int timer;

void 
#ifndef	AARCH64
__attribute__((interrupt("IRQ"))) 
interrupt()
#else
interrupt(int cause)
#endif
{

        struct rpiIrqController *rpiIRQController =
                (struct rpiIrqController *)RPI_INTERRUPT_CONTROLLER_BASE;

#ifdef	AARCH64
	if(cause !=5 ) {
		printf("B%x:%016lx\n",cause,rpiIRQController->IRQ_basic_pending);
	}

	if(!rpiIRQController->IRQ_basic_pending) {
		printf("B%x:%016lx\n",cause,rpiIRQController->IRQ_basic_pending);
	}
#endif

//	printf("1:%016lx\n",rpiIRQController->IRQ_pending_1 );
//	printf("2:%016lx\n",rpiIRQController->IRQ_pending_2 );
/*
	rpiIRQController->IRQ_pending_1;
	rpiIRQController->IRQ_pending_2;
	rpiIRQController->Enable_IRQs_1;
	rpiIRQController->Enable_IRQs_2;
	rpiIRQController->Enable_Basic_IRQs;
	rpiIRQController->Disable_IRQs_1;
	rpiIRQController->Disable_IRQs_2;
	rpiIRQController->Disable_Basic_IRQs;
*/

	if(rpiIRQController->IRQ_basic_pending & (0x01<<0)) {// 0
		timerHandler();
	};

	if(rpiIRQController->IRQ_basic_pending & (0x01<<1)) {// 1
		//mailboxHander();
	};
	if(rpiIRQController->IRQ_basic_pending & (0x01<<2)) {// 2
		//doorebell1Hadnler();
	};
	if(rpiIRQController->IRQ_basic_pending & (0x01<<3)) {// 3
		//doorebell2Hadnler();
	};

	if(rpiIRQController->IRQ_pending_1 & (0x01<<29)) {// 29
		uartHandler1();
	}

	if(rpiIRQController->IRQ_pending_2 & (0x01<<25)) {//25= 57-32 UART 
		uartHandler0();
	}


//	if(rpiIRQController->IRQ_pending_2 & (0x01<<17)) {/*49(17) GPIO0*/} 
//	if(rpiIRQController->IRQ_pending_2 & (0x01<<18)) {/*50(18) GPIO1*/} 
//	if(rpiIRQController->IRQ_pending_2 & (0x01<<19)) {/*51(19) GPIO2*/} 
//	if(rpiIRQController->IRQ_pending_2 & (0x01<<20)) {/*52(20) GPIO3*/} 
//	if(rpiIRQController->IRQ_pending_2 & (0x01<<21)) {/*53(21) I2C*/} 
//	if(rpiIRQController->IRQ_pending_2 & (0x01<<22)) {/*54(22) SPI*/} 
//	if(rpiIRQController->IRQ_pending_2 & (0x01<<23)) {/*55(23) PCM*/} 

}
