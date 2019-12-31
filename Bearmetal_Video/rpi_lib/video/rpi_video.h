#ifndef RPILIB_VIDEO_H
#define RPILIB_VIDEO_H
#include <stdint.h>

//typedef unsigned char 	     uint8_t;
//typedef unsigned int 	       uint16_t;
//typedef unsigned long int    uint32_t;

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
    uint32_t buf_size;   //write 0 to get value
} fb_info_t;

void mailbox_write(uint8_t chan, uint32_t msg);
uint32_t mailbox_read(uint8_t chan);
void fb_init(fb_info_t *fb_info);
void hline16(fb_info_t *fb_info, int x, int y, int l, uint32_t c);
void vline16(fb_info_t *fb_info, int x, int y, int l, uint32_t c);

#endif
