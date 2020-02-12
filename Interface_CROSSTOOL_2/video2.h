#ifndef RPILIB_VIDEO2_H
#define RPILIB_VIDEO2_H
#include <stdint.h>
// for RaspberryPi
#define MMIO_BASE       0x20000000
// for raspberryPi3
//#define MMIO_BASE       0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

/* メールボックスメッセージバッファ */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       IOREG(VIDEOCORE_MBOX + 0x00)
#define MBOX_POLL       IOREG(VIDEOCORE_MBOX + 0x10)
#define MBOX_SENDER     IOREG(VIDEOCORE_MBOX + 0x14)
#define MBOX_STATUS     IOREG(VIDEOCORE_MBOX + 0x18)
#define MBOX_CONFIG     IOREG(VIDEOCORE_MBOX + 0x1C)
#define MBOX_WRITE      IOREG(VIDEOCORE_MBOX + 0x20)

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_LAST   0

typedef struct _fb_info_t {
    unsigned int  display_w;  // ディスプレイの幅
    unsigned int  display_h;  // ディスプレイの高さ
    unsigned int  w;          // フレームバッファの幅
    unsigned int  h;          // フレームバッファの高さ
    unsigned int  row_bytes;  // 0を書き込んで値を取得する
    unsigned int  bpp;        // 1ピクセルあたりのビット数
    unsigned int  ofs_x;      // フレームバッファのxオフセット
    unsigned int  ofs_y;      // フレームバッファのyオフセット
    unsigned char *buf_addr;  // フレームバッファへのポインタ
    unsigned int  buf_size;   // フレームバッファのサイズ（バイト単位）
} fb_info_t;

#define COL8_000000 (((0x00>>3)<<11) + ((0x00>>3)<<6) + (0x00>>3))
#define COL8_008484 (((0x00>>3)<<11) + ((0x84>>3)<<6) + (0x84>>3))
#define COL8_848484 (((0x84>>3)<<11) + ((0x84>>3)<<6) + (0x84>>3))
#define COL8_C6C6C6 (((0xC6>>3)<<11) + ((0xC6>>3)<<6) + (0xC6>>3))
#define COL8_FFFFFF (((0xFF>>3)<<11) + ((0xFF>>3)<<6) + (0xFF>>3))

void mbox_write(unsigned char ch);
int mbox_read(unsigned char ch);
void lfb_init(fb_info_t *fb_info);
static inline void *coord2ptr(unsigned char *vram,
                              unsigned int pitch,
                              unsigned int bpp,
                              int x,
                              int y);
void boxfill8(unsigned char *vram,
              unsigned int c,
              unsigned int pitch,
              unsigned int bpp,
              int x0, int y0, int x1, int y1);

#endif
