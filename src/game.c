#include "game.h"
#include "string.h"
#include "graphics.h"
#include "keyboard.h"
#include "keymap.h"
#include "sound.h"

// ボールの状態
static int bx = 80, by = 50;
static int vx = 2, vy = 2; // 速度
// Score (bound count)
static int bc = 0;

void initGame() {
    bx = 70; by = 40;
}

static int is_playing = 0;
void updateGame() {
    // 壁での反射
    if (bx <= 0 || bx >= 152) {
        vx = -vx;
        bc ++;
    }
    if (by <= 0 || by >= 92) {
        vy = -vy;
        bc ++;
    }
    if (bc > 100) {
        // リセット
        bc = 0;
    }

    // 座標更新
    bx += vx;
    by += vy;

    // sound test
    if (getKeyState(KEY_SPACE) && !is_playing) {
        playSound(440);
        is_playing = 1;
    } else if (!getKeyState(KEY_SPACE) && is_playing) {
        stopSound();
        is_playing = 0;
    }
}

static unsigned char strBx[8] = {0};
static unsigned char strBy[8] = {0};
static unsigned char strBc[16] = {0};
static unsigned char strKc[8] = {0};
void drawGame() {
    int bcLength;

    // 背景を表示
    int i,j;
    for (j = 0; j < 10; j++) {
        for (i = 0; i < 16; i++) {
            setFgColor((i+j) & 0x0F);
            drawRect(i*10, j*10, 10, 10); 
        }
    }

    // ボールを描画
    setBgColor(-1);
    setFgColor(15);
    drawString(bx, by, "@"); 

    drawString(2, 2, "x:");
    drawString(2, 10, "y:");
    num2str(bx, strBx);
    drawString(18, 2, strBx);
    num2str(by, strBy);
    drawString(18, 10, strBy);
    for (int i = 0; i < 0x80; i++) {
        if (getKeyState(i)) {
            setFgColor((getColor(i*2, 98) + 2) & 0x0F);
            drawRect(i*2, 98, 2, 2);
        }
    }

    setFgColor(15);
    setBgColor(2);
    num2str(bc, strBc);
    bcLength = strlen(strBc);
    drawString(157 - (bcLength * 8), 90, strBc);
}
