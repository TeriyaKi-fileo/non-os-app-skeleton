#ifndef GRAPHICS_H
#define GRAPHICS_H

/**
 * 全景色の設定
 * @param {int} id - palette index (負数は透過)
 */
void setFgColor(int id);

/**
 * 背景色の設定
 * @param {int} id - palette index (負数は透過)
 */
void setBgColor(int id);

/**
 * 点を描画する
 * @param {int} x
 * @param {int} y
 */
void drawPixel(int x, int y);

/* 四角形を描画する */
void drawRect(int x, int y, int width, int height);

/* 直線を描画する */
void drawLine(int x0, int y0, int x1, int y1);

/* 数値を描画する */
void drawNumber(int x, int y, int number);

/* 文字を描画する */
void drawString(int x, int y, unsigned char* string);

#endif
