#include "io.h"
#include "core_graphics.h"
#include "graphics.h"
#include "string.h"
#include "misaki_font.h"

/* 仮想画面解像度 */
#define V_WIDTH 160
#define V_HEIGHT 100

static int abs(int n) { return n < 0 ? -n : n; }

static int fg_is_draw = 1; // 0:transparent, 1:draw
static int bg_is_draw = 1; // 0:transparent, 1:draw
static unsigned char fg_color = 0;
static unsigned char bg_color = 0;

static unsigned char bits[8] = {128, 64, 32, 16, 8, 4, 2, 1};

/* ---------------------------------------------------------- */
/* core */

static unsigned char vram[V_WIDTH * V_HEIGHT] = {0};
void initScreen() {
    unsigned char *s = vram; // 1ピクセル1バイトの作業配列
    unsigned int n = (V_WIDTH * V_HEIGHT) / 4; // 4:size of int(32bit)
    for (unsigned int i = 0; i < n; i++) {
        d[i] = (0xDE << 16) | 0xDE;
    }
}
void updateScreen() {
    volatile unsigned int *d = (volatile unsigned int *)0x000B8000;
    unsigned int *s = vram; // 1ピクセル1バイトの作業配列
    unsigned int n = (V_WIDTH * V_HEIGHT) / 4; // 4:size of int(32bit)

    while ((ioIn8(0x03da) & 0x08) == 0); // V-Sync待機
    while ((ioIn8(0x03da) & 0x08) != 0);

    // 16000ピクセル / 4 = 4000回ループ
    for (unsigned int i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

/* ---------------------------------------------------------- */
/* common */

void setFgColor(int id) {
    if (id < 0) {
        fg_is_draw = 0;
    } else {
        fg_is_draw = 1;
        fg_color = (unsigned char)(id & 0x0f);
    }
}

void setBgColor(int id) {
    if (id < 0) {
        bg_is_draw = 0;
    } else {
        bg_is_draw = 1;
        bg_color = (unsigned char)(id & 0x0f);
    }
}

void _drawPixel(int x, int y, unsigned char color_id) {
    unsigned char c;
    unsigned char col = color_id;
    unsigned int index;
    // 画面外への書き込みを防止
    if (x >= 0 && x < V_WIDTH && y >= 0 && y < V_HEIGHT) {
        index = (y * V_WIDTH) + (x | 0x01);
        c = vram[index]
        if (x & 0x01) {
            col = col & 0x0F;
            c = c & 0xF0;
        } else {
            col = col << 4;
            c = c & 0x0F;
        }
        vram[index] = (color_id | c);
    }
}
void drawPixel(int x, int y) {
    if (fg_is_draw) {
        _drawPixel(x, y, fg_color);
    }
}

void drawRect(int x, int y, int width, int height) {
    if (!fg_is_draw) {
        return;
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            drawPixel(x + j, y + i);
        }
    }
}

void drawLine(int x0, int y0, int x1, int y1) {
    if (!fg_is_draw) {
        return;
    }
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        drawPixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static unsigned char num_str[16] = {0};
void drawNumber(int x, int y, int number) {
    int i;
    for (i = 0; i < 16; i++) {
        num_str[i] = 0;
    }
    num2str(number, num_str);
    drawString(x, y, num_str);
}

void drawString(int x, int y, unsigned char* string) {
    int index, baseX, i, j;
    unsigned char *cs;
    for (index = 0; string[index] != 0; index++) {
        baseX = index * 8 + x;
        cs = getMisakiPointer(string[index]);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                if ((cs[i] & bits[j] & 0xff) > 0) {
                    if (fg_is_draw) {
                        _drawPixel(baseX + j, y + i, fg_color);
                    }
                } else {
                    if (bg_is_draw) {
                        _drawPixel(baseX + j, y + i, bg_color);
                    }
                }
            }
        }
    }
}
