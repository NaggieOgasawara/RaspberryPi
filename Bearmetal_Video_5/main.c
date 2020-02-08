#include "rpi_lib/rpi.h"
//#include "rpi_lib/video/rpi_video.h"
#include <stdio.h>
//#include <stdint.h>

// static fb_info_t fb_info = {1920, 1080, 480, 270, 0, 16, 0, 0, 0, 0};

int main(int argc, char **argv) {
    int wait;
//
    rpi_init();

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

    drawGrid();

//    printf("HelloWorld\r\n");

    wait = 1;
    while(wait){
  		// 1秒ごとにHelloWorld送信
//  		printf("HelloWorld\r\n");
  		delay(1000);
    }

// we must not return
/*
    for (;;) {
    }
*/
    return 0;
}
