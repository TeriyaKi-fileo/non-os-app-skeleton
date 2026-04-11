#include "io.h"
#include "core_keyboard.h"
#include "core_graphics.h"
#include "game.h"

void main() {
    gameInit(); // 注入されたゲームの初期化

    for (;;) {
        updateKeyInput();
        
        gameUpdate(); // ロジック実行
        gameDraw();   // 描画実行

        updateScreen(); // VRAM転送
    }
}
