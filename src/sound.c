#include "io.h"
#include "sound.h"

/* O4の音階(黒鍵含む) */
static const unsigned int OCTAVE4_FULL_TABLE[] = {
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
};
/* O4の白鍵の位置 */
static const unsigned char KEY_STEPS[] = {0, 2, 4, 5, 7, 9, 11};

/**
 * 音階で鳴らす
 * octave: about 0 - 8 ?
 * C=0, D=1, E=2, ..., B=6
 * isSharp= 0 or 1
 */
void playNote(unsigned char octave, unsigned char note, unsigned char isSharp) {
    unsigned char noteIndex = KEY_STEPS[note % 7] + isSharp;
    // 基準オクターブの周波数を取得
    unsigned int freq = OCTAVE4_FULL_TABLE[noteIndex % 12];
    // オクターブ移動をシフト演算（2のn乗）で解決
    int shift = octave - 4;
    if (shift > 0) {
        playSound(freq << shift);
    } else {
        playSound(freq >> -shift);
    }
}

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
