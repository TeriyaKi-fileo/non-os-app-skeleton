# non os app skeleton
MBR起動で直接セクタ読み込みして動くアプリの雛形

## 使い方
- 現状をベースとして、肉付けして使う

## 雛形の仕様
- 画面
  - 160 x 100 (Mode 03H)
    - AX:0x1110 の処理の副作用を利用しています
  - 16 色 (初期値はシステムカラー)
    - 00:black(#000000)
    - 01:blue(#000080)
    - 02:green(#008000)
    - 03:cyan(#008080)
    - 04:red(#800000)
    - 05:magenta(#800080)
    - 06:brown(#808000)
    - 07:light grey(#c0c0c0)
    - 08:grey(#808080)
    - 09:light blue(#0000ff)
    - 10:light green(#00ff00)
    - 11:light cyan(#00ffff)
    - 12:light red(#ff0000)
    - 13:light magenta(#ff00ff)
    - 14:yellow(#ffff00)
    - 15:bright white(#ffffff)

- 音声
  - Beep (周波数または音階を指定)

- 動作
  - フロッピー起動想定で、起動時にセクタ２以降をすべてメモリに読み込む
  - 読み込んだのち、32ビットプロテクトモードでエントリーポイントから実行
    - つまり実機だととんでもなく起動に時間がかかります

## 設計方針
### ヘッダによる擬似名前空間
- 物理的にディレクトリを分割せずフラットな構成を維持する。
- 代わりに、ヘッダファイルの使い分けによって「汎用API」と「カーネル特権API」の名前空間を論理的に分離している。
  - アプリの特性上、ソースコードが多すぎて把握ができないといった状況にはならないという想定

> [!IMPORTANT]
> - [module].h: 汎用的なアクセスを許可するインターフェース。
> - core_[module].h: カーネル中枢からのみ利用可能な制御インターフェース。
>   - これにより、コードの依存関係を静的に可視化し、設計の汚染を防ぐ。

## 想定している環境
- 開発環境
  - nasm: アセンブリを機械語にする。
  - gcc / ld: C言語のコンパイルとリンク。
  - make: ビルド工程の自動化。
  - qemu-system-x86: 仮想PC。動作確認用。
  - bochs: より細かい挙動を確認したいときに使用。

- 動作環境
  - 386以降のPC/AT互換機

## 構築
- sudo apt update
- sudo apt install nasm gcc make qemu-system-x86
- sudo apt install bochs bochs-sdl bochsbios vgabios
