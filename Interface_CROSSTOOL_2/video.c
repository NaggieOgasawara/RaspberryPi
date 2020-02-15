#include "video.h"

void mbox_write(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));

    /* メールボックスに書き込むまで待つ */
    do{
        asm volatile("nop");
    }while(MBOX_STATUS & MBOX_FULL);

    /* メッセージのアドレスをチャネル識別子を持つメールボックスに書き込む */
    MBOX_WRITE = r;
}

int mbox_read(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));
    while(1) {
        /* 応答を待つ */
        do{
            asm volatile("nop");
        }while(MBOX_STATUS & MBOX_EMPTY);

        /* メッセージに対する応答か判定 */
        if(r == MBOX_READ){
            /* 有効な成功応答か判定 */
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}

void lfb_init(fb_info_t *fb_info){
    /* メールボックスメッセージバッファを0クリア */
    for(int i = 0; i < 36; i++){
        mbox[i] = 0;
    }

    mbox[0] = 112;           // 合計サイズ
    mbox[1] = MBOX_REQUEST;  // 要求コード：プロセス要求

    mbox[2] = 0x48003;  //ディスプレイの幅/高さを設定する
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = fb_info->display_w;
    mbox[6] = fb_info->display_h;

    mbox[7] = 0x48004;  // フレームバッファの幅/高さを設定する
    mbox[8] = 8;
    mbox[9] = 0;
    mbox[10] = fb_info->w;
    mbox[11] = fb_info->h;

    mbox[12] = 0x48005; // 深度を設定する
    mbox[13] = 4;
    mbox[14] = 0;
    mbox[15] = fb_info->bpp;

    mbox[16] = 0x40008; // ピッチを取得
    mbox[17] = 4;
    mbox[18] = 0;
    mbox[19] = 0;

    mbox[20] = 0x40001; // フレームバッファを取得し、要求に応じて配置を取得します。
    mbox[21] = 8;
    mbox[22] = 0;
    mbox[23] = 0;
    mbox[24] = 0;

    mbox[25] = MBOX_TAG_LAST;

    /* MailBoxへ書き込む */
    mbox_write(MBOX_CH_PROP);

    /* MailBoxから読み出す */
    if(mbox_read(MBOX_CH_PROP) && mbox[15] == 16 && mbox[23] != 0) {
        mbox[23] &= 0x3FFFFFFF;
        fb_info->display_w = mbox[5];
        fb_info->display_h = mbox[6];
        fb_info->bpp       = mbox[15];
        fb_info->row_bytes = mbox[19];
        fb_info->buf_addr  = (void*)((unsigned long)mbox[23]);
    }
}

static inline void *coord2ptr(unsigned char *vram,
                              unsigned int pitch,
                              unsigned int bpp,
                              int x,
                              int y) {
    return (void *)(vram + ((bpp + 7) >> 3) * x + pitch * y);
}

void boxfill8(unsigned char *vram,
              unsigned int c,
              unsigned int pitch,
              unsigned int bpp,
              int x0, int y0, int x1, int y1){
    int x, y;
    unsigned short int *p;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++){
            plot( vram, c, pitch, bpp, x, y );
//            p = (unsigned short int *)coord2ptr(vram, pitch, bpp, x, y);
//            *p = c;
        }
    }
}

void line(unsigned char *vram,
              unsigned int c,
              unsigned int pitch,
              unsigned int bpp,
              int sx, int sy, int ex, int ey ) {
    int cx, cy; //  current pos
    int dx, dy; //
    int ddx, ddy;
    int bunsi = 0;

    cx = sx;
    cy = sy;
    //  dx = abs( sx - ex );
    if( sx > ex )
      dx = sx - ex;
    else
      dx = ex - sx;
    //  dy = abs( sy - ey );
    if ( sy > ey )
      dy = sy - ey;
    else
      dy = ey - sy;
    ddx = ( ex - sx ) / dx;
    ddy = ( ey - sy ) / dy;

    plot( vram, c, pitch, bpp, cx, cy );
    if( dx > dy ) {
      for ( int i = 0; i < dx; i++ ) {
        bunsi = bunsi + dy;
        if ( bunsi >= dx ) {
          bunsi = bunsi - dx;
          cy = cy + ddy;
        }
        cx = cx + ddx;
        plot( vram, c, pitch, bpp, cx, cy );
      }
    } else {
      for ( int i = 0; i < dy; i++ ) {
        bunsi = bunsi + dx;
        if( bunsi >= dy ) {
          bunsi = bunsi - dy;
          cx = cx + ddx;
        }
        cy = cy + ddy;
        plot( vram, c, pitch, bpp, cx, cy );
      }
    }
}

void plot(unsigned char *vram,
          unsigned int c,
          unsigned int pitch,
          unsigned int bpp,
          int x,
          int y) {
    unsigned short int *p;
    p = (unsigned short int *)coord2ptr(vram, pitch, bpp, x, y);
    *p = c;
}
