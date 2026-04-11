#ifndef KEYMAP_H
#define KEYMAP_H

// スキャンコードをそのまま書くと分かりづらいので、マッピング情報を用意する

// --- 制御キー ---
#define KEY_ESC          0x01
#define KEY_BACKSPACE    0x0e
#define KEY_TAB          0x0f
#define KEY_ENTER        0x1c

// --- 共通化された修飾キー ---
// PS/2 Set 1では左右で異なる信号が来ますが、0xe0を無視することで
// どちらを押しても以下のIDで反応するように整理します。
#define KEY_CTRL         0x1d
#define KEY_ALT          0x38

// --- 個別管理が必要な修飾キー ---
#define KEY_L_SHIFT   0x2a
#define KEY_R_SHIFT  0x36

// --- その他 ---
#define KEY_SPACE        0x39

// --- 数字キー ---
#define KEY_0            0x0b
#define KEY_1            0x02
#define KEY_2            0x03
#define KEY_3            0x04
#define KEY_4            0x05
#define KEY_5            0x06
#define KEY_6            0x07
#define KEY_7            0x08
#define KEY_8            0x09
#define KEY_9            0x0a

// --- アルファベット A-Z ---
#define KEY_A            0x1e
#define KEY_B            0x30
#define KEY_C            0x2e
#define KEY_D            0x20
#define KEY_E            0x12
#define KEY_F            0x21
#define KEY_G            0x22
#define KEY_H            0x23
#define KEY_I            0x17
#define KEY_J            0x24
#define KEY_K            0x25
#define KEY_L            0x26
#define KEY_M            0x32
#define KEY_N            0x31
#define KEY_O            0x18
#define KEY_P            0x19
#define KEY_Q            0x10
#define KEY_R            0x13
#define KEY_S            0x1f
#define KEY_T            0x14
#define KEY_U            0x16
#define KEY_V            0x2f
#define KEY_W            0x11
#define KEY_X            0x2d
#define KEY_Y            0x15
#define KEY_Z            0x2c

// --- 矢印キー ---
// 拡張コードを無視するようにすれば 0x48 等で共通化されます
#define KEY_UP           0x48
#define KEY_LEFT         0x4b
#define KEY_RIGHT        0x4d
#define KEY_DOWN         0x50

#endif
