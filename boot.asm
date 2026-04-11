[bits 16]
org 0x7c00

start:
    ; CGA03H / extra
    ; 160x100 16 colors / VRAM = B800:0000 - B800:7FFF.
    ; 2 pixels per 2bytes.
    ; 
    ; 386以降の端末を想定しているので、VGA前提でボードチェックしていない
    ;
    ; 76543210 76543210
    ; 11011110 ....::::
    ; |        |   +-- odd pixel color palette 0..F
    ; |        +----- even pixel color palette 0..F
    ; +------ character $DE = #_ (Left half block)

    ; 1. 標準テキストモード (80x25, 16色) に設定
    mov ax, 0x0003
    int 0x10

    ; 2. font adjust
    mov ax, 0x1110
    mov bx, 0x0400
    mov cx, 0x0001
    mov dx, 0x0000
    int 0x10

    ; 3. 輝度ビット(Blink)を無効化し、背景色でも16色使えるようにする
    mov ax, 0x1003
    mov bl, 0x00
    int 0x10

    ; 4. カーソルを非表示にする
    mov ah, 0x01
    mov cx, 0x2020
    int 0x10

    ; --- フロッピー全域(約1.44MB)を 0x10000 へ読み込むループ ---
    mov ax, 0x1000      ; 読み込み先セグメント
    mov es, ax
    mov bx, 0           ; オフセット
    
    mov cx, 0x0002      ; CH=0(トラック), CL=2(セクタ2から開始)
    mov dx, 0x0000      ; DH=0(ヘッド), DL=0(ドライブ0)

read_loop:
    mov ah, 0x02
    mov al, 1           ; 1セクタずつ確実に読む (低速だが確実)
    int 0x13
    jc  error

    ; 次のセクタへ準備
    add bx, 512         ; 1セクタ分オフセットを進める
    jnz next_sector     ; 64KB境界を超えていなければ次へ
    mov ax, es          ; 64KB超えたらセグメントを更新
    add ax, 0x1000
    mov es, ax

next_sector:
    inc cl              ; セクタ番号を増やす
    cmp cl, 19          ; 18セクタ(1トラック分)読んだか？
    jne check_end

    mov cl, 1           ; セクタを1に戻す
    inc dh              ; ヘッドを裏面(1)へ
    cmp dh, 2           ; 両面読んだか？
    jne check_end

    mov dh, 0           ; ヘッドを表面(0)に戻す
    inc ch              ; 次のシリンダへ

check_end:
    ; 合計読み込み回数をカウントするか、トラック数で判定
    ;cmp ch, 40          ; 40シリンダ(全容量)まで読んだら終了
    cmp ch, 28          ; 何も考えずに全トラックを読んだらVRAM破壊する容量になるので、28シリンダまでにする
    jne read_loop

    ; --- 32bit移行 ---
    cli
    ; --- A20ラインを有効化 ---
    in al, 0x92
    or al, 2
    out 0x92, al
    ; --- --- ---
    lgdt [gdtr]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp 0x08:init_pm

[bits 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000    ; スタックを1MB以下に確保

    ; --- High Memory (0x100000) への一括転送 ---
    ; 0x10000 に読み込んだデータを 0x100000 へコピー
    mov esi, 0x10000    ; 転送元
    mov edi, 0x100000   ; 転送先 (1MB)
    ; 1シリンダあたり18セクタ * 2ヘッドあるので、
    ; それをもとに読み込んだトラック数からセクタ数を計算（2HDなら１シリンダ２トラック）
    ; ただし転送容量はブートセクタ１をマイナスしておく
    ;mov ecx, (1439 * 512) / 4 ; すべて(4バイト単位) --- 18セクタ * 2ヘッド * 40シリンダ = 1440 - 自身1
    mov ecx, (1007 * 512) / 4 ; 28シリンダ分(4バイト単位) --- 18 * 2 * 28 - 1
    rep movsd

    ; --- 転送後は元の場所(0x10000)をクリア ---
    mov edi, 0x10000
    xor eax, eax
    mov ecx, (1007 * 512) / 4 ; 転送したのと同じサイズ
    rep stosd                 ; 0で塗りつぶす

    jmp 0x100000        ; 1MB地点のカーネルへジャンプ

error:
    jmp $

; GDT
align 8
gdt_base:
    dd 0, 0
    dw 0xffff, 0x0000, 0x9a00, 0x00cf ; コードセグメント
    dw 0xffff, 0x0000, 0x9200, 0x00cf ; データセグメント
gdt_end:

gdtr:
    dw gdt_end - gdt_base - 1
    dd gdt_base

; MBR用の埋め
times 510-($-$$) db 0
dw 0xaa55
