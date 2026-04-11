#include "io.h"
#include "core_keyboard.h"
#include "keyboard.h"

// キーボードの状態配列
static unsigned char key_state[256] = {0};

/* core */

void updateKeyInput(void) {
    // データがある間ループ
    while (ioIn8(0x64) & 0x01) {
        unsigned char data = ioIn8(0x60); // 読み取る
        
        // 状態変更
        key_state[data & 0x7f] = (data & 0x80) ? 0 : 1;
    }
}

/* common */

unsigned char getKeyState(unsigned char index) {
    if (index > 127) {
        return 0;
    }
    return key_state[index & 0x7f];
}
