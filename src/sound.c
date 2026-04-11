#include "io.h"
#include "sound.h"

void playSound(unsigned int frequency) {
    unsigned int div = 1193180 / frequency;

    // PITを設定
    ioOut8(0x43, 0xb6);
    ioOut8(0x42, (unsigned char)(div));
    ioOut8(0x42, (unsigned char)(div >> 8));

    // PCスピーカーをON (ポート0x61のビット0と1をセット)
    unsigned char tmp = ioIn8(0x61);
    ioOut8(0x61, tmp | 0x03);
}

void stopSound(void) {
    // PCスピーカーをOFF
    unsigned char tmp = ioIn8(0x61);
    ioOut8(0x61, tmp & 0xFC);
}
