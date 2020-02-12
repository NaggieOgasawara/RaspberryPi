#include "video.h"

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

static fb_info_t fb_info ={1920, 1080, 480, 270, 0, 16, 0, 0, 0, 0};

void fb_init() {
    fb_info.buf_addr = 0;
    fb_info.buf_size = 0;
    fb_info.row_bytes = 0;
    while(fb_info.buf_addr == 0) {
        mailbox_write(1, (uint32_t) (&fb_info + 0x40000000));
        mailbox_read(1);
    }
}

static inline void *coord2ptr( int x, int y) {
    return (void *) (fb_info.buf_addr                   \
                     + (fb_info.bpp + 7 >> 3) * x       \
                     + fb_info.row_bytes * y);
}

void hline16( int x, int y, int l, uint32_t c) {
    uint16_t *p = (uint16_t *) coord2ptr( x, y );
    if (fb_info.w < l + x) {
        l = fb_info.w - x;
    }
    for(int i = 0; i < l; i++) {
        *p++ = c;
    }
}

void vline16( int x, int y, int l, uint32_t c) {
    uint16_t *p = (uint16_t *) coord2ptr( x, y );
    if (fb_info.h < l + y) {
        l = fb_info.h - y;
    }
    for(int i = 0; i < l; i++) {
        *p = c;
        p += fb_info.row_bytes >> 1;
    }
}

void drawGrid(){
    for(int i = 0; i < fb_info.h; i += 8) {
        hline16( 0, i, fb_info.w, 0x5050 * i);
        //hline16(0, i, fb_info.w, 0xffffffff );
    }


    for(int i = 0; i < fb_info.w; i += 8) {
        vline16( i, 0, fb_info.h, 0xa0a0 * i);
        //vline16(i, 0, fb_info.h, 0xffffffff );
    }
}
