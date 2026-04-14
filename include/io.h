#ifndef IO_H
#define IO_H

/* データの出力 */
void ioOut8(int port, int data);

/* データの入力 */
unsigned char ioIn8(int port);

/* VSync待ち */
void ioVsyncWait(void);

#endif
