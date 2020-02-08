//#include "rpi_lib/rpi.h"
#include "rpi_lib/video/rpi_video.h"
#include <stdio.h>
//#include <stdint.h>

static fb_info_t fb_info = {1920, 1080, 480, 270, 0, 16, 0, 0, 0, 0};

int main(int argc, char **argv) {
    int i;

/*
    rpi_init();
*/

    fb_init(&fb_info);

    for(i = 0; i < fb_info.h; i += 8) {
        hline16(&fb_info, 0, i, fb_info.w, 0x5050 * i);
        //hline16(0, i, fb_info.w, 0xffffffff );
    }


    for(i = 0; i < fb_info.w; i += 8) {
        vline16(&fb_info, i, 0, fb_info.h, 0xa0a0 * i);
        //vline16(i, 0, fb_info.h, 0xffffffff );
    }

/*
    while(1){
  		// 1秒ごとにHelloWorld送信
  		printf("HelloWorld\r\n");
  		delay(1000);
  	}
*/

// we must not return
    for (;;) {
    }

    return 0;
}
