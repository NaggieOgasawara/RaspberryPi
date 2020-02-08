/*
              no syscall    syscall
  Release     o             x
  Debug
*/
#include "rpi_lib/rpi.h"
#include <stdio.h>

int main(int argc, char **argv) {
    int wait;
//
    rpi_init();

/*  デバッグ用
    // JTAG用設定
    // 3.3V			: ARM_VREF
    // GPIO22 (ALT4): ARM_TRST
    // GPIO4  (ALT5): ARM_TDI
    // GPIO27 (ALT4): ARM_TMS
    // GPIO25 (ALT4): ARM_TCK
    // GPIO24 (ALT4): ARM_TDO
    // GND			: ARM_GND
    pinMode(22, ALT4);
    pinMode(4, ALT5);
    pinMode(27, ALT4);
    pinMode(25, ALT4);
    pinMode(24, ALT4);

    wait = 1;

    while( wait ){
    }
*/

//    drawGrid();

    printf("HelloWorld\r\n");

    wait = 1;
    while(wait){
  		// 1秒ごとにHelloWorld送信
  		printf("HelloWorld\r\n");
  		delay(1000);
    }

// we must not return
/*
    for (;;) {
    }
*/
    return 0;
}
