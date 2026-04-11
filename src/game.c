#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include "keymap.h"
#include "sound.h"

// ボールの状態
static int bx = 80, by = 50;
static int vx = 2, vy = 2; // 速度
// Score (bound count)
static int bc = 0;

void gameInit() {
    bx = 80; by = 50;
}

static int is_playing = 0;
void gameUpdate() {
    // 壁での反射
    if (bx <= 0 || bx >= 150) {
        vx = -vx;
        bc ++;
    }
    if (by <= 0 || by >= 90) {
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


    // 描画前に背景を消去
    setFgColor(0);
    drawRect(0, 0, 160, 100); 

    // 
    int i;
    for (i = 0; i < 256; i++) {
        setFgColor(getKeyState(i & 0xff) ? 15 : 0);
        drawPixel(i, 4);
    }
}

void gameDraw() {
    // ボールを描画
    setFgColor(15);
    drawRect(bx, by, 10, 10); 
    setBgColor(-1);
    drawString(2, 2, "x:");
    drawString(2, 10, "y:");
    drawNumber(18, 2, bx);
    drawNumber(18, 10, by);
    setBgColor(2);
    drawNumber(150, 90, bc);
}
