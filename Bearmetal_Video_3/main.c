#include "rpi_lib/rpi.h"
#include <stdio.h>

int main(int argc, char **argv) {

    rpi_init();

    drawGrid();

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
