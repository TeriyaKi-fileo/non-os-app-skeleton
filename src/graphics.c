#include "io.h"
#include "core_graphics.h"
#include "graphics.h"
#include "string.h"
#include "misaki_font.h"

/* 仮想画面解像度 */
/* 80 columns x 2 dot */
#define V_WIDTH 160
/* 25 rows x 4 planes */
#define V_HEIGHT 100

static int abs(int n) { return n < 0 ? -n : n; }

static int fg_is_draw = 1; // 0:transparent, 1:draw
static int bg_is_draw = 1; // 0:transparent, 1:draw
static unsigned char fg_color = 0;
static unsigned char bg_color = 0;

static unsigned char bits[8] = {128, 64, 32, 16, 8, 4, 2, 1};

/* ---------------------------------------------------------- */
/* core */

static unsigned char vram[V_WIDTH * V_HEIGHT] __attribute__((aligned(4))) = {0};
void initScreen() {
    volatile unsigned int *d = (volatile unsigned int *)vram;
    unsigned int n = (V_WIDTH * V_HEIGHT) / 4; // 4:size of int(32bit)
    for (unsigned int i = 0; i < n; i++) {
        d[i] = (0xDE << 16) | 0xDE; // 最初は前景・背景ともに黒（０）でよい
    }
    updateScreen();
}
void updateScreen() {
    ioVsyncWait();
    // 0xB8000 はテキストVRAMのアドレス
    __asm__ __volatile__ (
        "cld; rep movsl"
        :
        : "S"(vram), "D"(0xB8000), "c"(4000) // 160*100 / 4 = 4000
        : "memory"
    );
}

/* ---------------------------------------------------------- */
/* common */

/**
 * id: 0-15
 * r,g,b: 0-255
 */
void setPalette(int id, unsigned char r, unsigned char g, unsigned char b) {
    ioCli();
    ioOut8(0x03c8, id & 0x0F);
    ioOut8(0x03c9, r >> 2);
    ioOut8(0x03c9, g >> 2);
    ioOut8(0x03c9, b >> 2);
    ioSti();
}

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

static inline void _drawPixel(int x, int y, unsigned char color_id) {
    unsigned char c;
    unsigned char col = color_id;
    unsigned int index;
    // 画面外への書き込みを防止
    if (x >= 0 && x < V_WIDTH && y >= 0 && y < V_HEIGHT) {
        index = (y * V_WIDTH) + (x | 0x01);
        c = vram[index];
        if (x & 0x01) {
            col = col & 0x0F;
            c = c & 0xF0;
        } else {
            col = col << 4;
            c = c & 0x0F;
        }
        vram[index] = (col | c);
    }
}
void drawPixel(int x, int y) {
    if (fg_is_draw) {
        _drawPixel(x, y, fg_color);
    }
}

unsigned char getColor(int x, int y) {
    unsigned char c;
    if (x >= 0 && x < V_WIDTH && y >= 0 && y < V_HEIGHT) {
        c = vram[(y * V_WIDTH) + (x | 0x01)];
        if (x & 0x01) {
            return c & 0x0F;
        } else {
            return (c >> 4) & 0x0F;
        }
    } else {
        return 0;
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
