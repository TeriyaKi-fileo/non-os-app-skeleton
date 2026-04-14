#include "io.h"
#include "core_keyboard.h"
#include "core_graphics.h"
#include "game.h"

void main() {
    initGame(); // 注入されたゲームの初期化
    initScreen(); // 画面初期化

    for (;;) {
        updateKeyInput();
        
        updateGame(); // ロジック実行
        drawGame();   // 描画実行

        updateScreen(); // VRAM転送
    }
}
