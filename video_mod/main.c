#include <stdint.h>
//#include <stdio.h>
#include "rpi_lib/rpi.h"

#define IOREG(X)  (*(volatile uint32_t *) (X))

#define MAILBOX0_FIFO   IOREG(0x2000B880)
#define MAILBOX0_POLL   IOREG(0x2000B890)
#define MAILBOX0_SENDER IOREG(0x2000B894)
#define MAILBOX0_STATUS IOREG(0x2000B898)
#define MAILBOX0_CONFIG IOREG(0x2000B89C)
#define MAILBOX1_FIFO   IOREG(0x2000B8A0)
#define MAILBOX1_POLL   IOREG(0x2000B8B0)
#define MAILBOX1_SENDER IOREG(0x2000B8B4)
#define MAILBOX1_STATUS IOREG(0x2000B8B8)
#define MAILBOX1_CONFIG IOREG(0x2000B8BC)

#define MAIL_FULL      0x80000000
#define MAIL_EMPTY     0x40000000

#define NULL ((void *)0)

void mailbox_write(uint8_t chan, uint32_t msg) {
    if ((msg & 0xfU) == 0) {
        while ((MAILBOX1_STATUS & MAIL_FULL) != 0) {
        }
        MAILBOX1_FIFO = msg | chan;
    }
}

uint32_t mailbox_read(uint8_t chan) {
    uint32_t data;
    do {
        while (MAILBOX0_STATUS & MAIL_EMPTY) {
        }
    } while (((data = MAILBOX0_FIFO) & 0xfU) != chan);
    return data >> 4;
}

typedef volatile struct                                          \
__attribute__((aligned(16))) _fb_info_t {
    uint32_t display_w;  //write display width
    uint32_t display_h;  //write display height
    uint32_t w;          //write framebuffer width
    uint32_t h;          //write framebuffer height
    uint32_t row_bytes;  //write 0 to get value
    uint32_t bpp;        //write bits per pixel
    uint32_t ofs_x;      //write x offset of framebuffer
    uint32_t ofs_y;      //write y offset of framebuffer
    uint32_t buf_addr;   //write 0 to get value
    uint32_t buf_size;     //write 0 to get value
} fb_info_t;

void fb_init(fb_info_t *fb_info) {
    fb_info->buf_addr = 0;
    fb_info->buf_size = 0;
    fb_info->row_bytes = 0;
    while(fb_info->buf_addr == 0) {
        mailbox_write(1, (uint32_t) (fb_info + 0x40000000));
        mailbox_read(1);
    }
}

const int16_t chaPtr[8* 2] = { 0b0000000000000000,
                               0b0000000000000000,
                               0b0000000000000000,
                               0b0000000000000000,
                               0b0000000000000000,
                               0b0000000000000000,
                               0b0000000000000000,
                               0b0000000000000000,

                               0b0000000000000000,   //  A
                               0b0011111000000000,
                               0b0111001100000000,
                               0b0111001100000000,
                               0b0111111100000000,
                               0b0111001100000000,
                               0b0111001100000000,
                               0b0111001100000000 };

static int8_t mychar[][8] =
      { { 0, 0, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 0, 0, 1 },
        { 0, 1, 1, 1, 1, 0, 0, 1 },
        { 0, 1, 1, 1, 1, 0, 0, 1 },
        { 0, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1, 0, 0, 1 },
        { 0, 1, 1, 1, 1, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0 } };

static fb_info_t fb_info = {1920, 1080, 480, 270, 0, 32, 0, 0, 0, 0};

static inline void *coord2ptr(int x, int y) {
    return (void *) (fb_info.buf_addr                   \
                     + (fb_info.bpp + 7 >> 3) * x       \
                     + fb_info.row_bytes * y);
}

//void point( int x, int y, uint32_t c) {
void point( int x, int y, uint32_t c) {
//  uint16_t *p = (uint16_t *) coord2ptr(x, y);
  int16_t *p = (int16_t *) coord2ptr(x, y);
  *p = c;
}

//  8*8 dots のキャラクタ表示
void drawchar( int x, int y, int cha, uint32_t c ) {
    for(int dy = 0; dy < 8; dy++) {
      for(int dx = 0; dx < 8; dx++ ) {
        if( mychar[ cha * 8 + dy ][ dx ] == 1 )
          point( x + dx, y + dy, c );
      }
    }
}

//  8*8 dots のキャラクタ表示
void drawcha( int x, int y, int cha ) {

    for( int dy = 0; dy < 8; dy++ ) {
      uint16_t a = chaPtr[ cha * 8 ];
      uint16_t b = 0b1000000000000000;
      uint16_t c;
      // printf("a:%x b:%x c:%x\r\n",a,b,c);
      for( int dx = 0; dx < 8; dx++) {
        c = a & b;
        if( c != 0)
          point( x + dx, y + dy, 0xffffffff );
        b = b >> 1;
      }
    }
}

//  ***************************************
//  画面の消去
void cls() {
  uint32_t *p = (uint32_t *) coord2ptr( 0, 0 );
  for(int i = 0; i < fb_info.w * fb_info.h; i++ ) {
    *p++ = 0x00000000;
  }
}

//void hline16(int x, int y, int l, uint32_t c) {
void hline16(int x, int y, int l, int16_t c) {
//    uint16_t *p = (uint16_t *) coord2ptr(x, y);
    int16_t *p = (int16_t *) coord2ptr(x, y);
    if (fb_info.w < l + x) {
        l = fb_info.w - x;
    }
    for(int i = 0; i < l; i++) {
        *p++ = c;
    }
}

//void vline16(int x, int y, int l, uint32_t c) {
// tate
void vline16(int x, int y, int l, unsigned int c) {
    uint16_t *p = (uint16_t *) coord2ptr(x, y);
    if (fb_info.h < l + y) {
        l = fb_info.h - y;
    }
    for(int i = 0; i < l; i++) {
        *p = c;
//        p += fb_info.row_bytes >> 1;
        p += fb_info.row_bytes >> 2;
    }
}

int main(int argc, char **argv) {
    int i;

//    rpi_init();
    fb_init(&fb_info);

//    for(i = 0; i < fb_info.h; i += 20) {
//        hline16(0, i, fb_info.w, 0x5050 * i);
//    }
//    for(i = 0; i < fb_info.w; i += 8) {
//        vline16(i, 0, fb_info.h, 0xa0a0 * i);
//    }

    point(100,100,0x7fff);
    point(101,101,0x7fff);
    point(102,102,0x7fff);
    point(103,103,0x7fff);
    point(104,104,0x7fff);

    drawchar( 150, 150, 0, 0xffffffff );

//    printf("Test Start\r\n");
//    delay(1000);
    drawcha(70,100,1);
//    delay(1000);
//    printf("Test End\r\n");
    for (;;) {
    }

    return 0;
}
