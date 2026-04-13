#ifndef IO_H
#define IO_H

/* データの出力 */
void ioOut8(int port, int data);

/* データの入力 */
unsigned char ioIn8(int port);

/* VSync待ち */
void ioVsyncWait(void);

/* CPUを休止させる（割り込み待ち） */
void ioHlt(void);

/* 割り込み禁止・許可 */
void ioCli(void);
void ioSti(void);

#endif
