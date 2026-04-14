[bits 16]
org 0x7c00

start:
    ; 03H
    ; 160x100 16 colors / VRAM = B800:0000 - B800:7FFF.
    ; 2 pixels per 2bytes.
    ; 
    ; 386以降の端末を想定しているので、VGA前提。
    ; 時代的に問題ないだろうとボードチェックしていない。
    ;
    ; 76543210 76543210
    ; 11011110 ....::::
    ; |        |   +-- odd pixel color palette 0..F
    ; |        +----- even pixel color palette 0..F
    ; +------ character $DE = #_ (Left half block)

    ; 1. 標準テキストモード (80x25, 16色) に設定
    mov ax, 0x0003
    int 0x10

    ; 2. font adjust(フォント再設定機能を利用して副作用で高さを1/4にする)
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

    ; --- A20ラインを有効化 ---
    in al, 0x92
    or al, 2
    out 0x92, al

    ; --- Unreal Mode 移行処理 ---
    cli
    lgdt [gdtr]
    mov eax, cr0
    or  al, 1
    mov cr0, eax        ; プロテクトモードへ一時移行
    jmp pmode_entry
pmode_entry:
    mov bx, 0x10        ; データセグメント記述子(0x10)を選択
    mov ds, bx          ; 各セグメントの隠しリミットを4GBに拡張
    mov es, bx
    mov ss, bx
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax        ; リアルモードへ復帰
    jmp 0:back_to_real
back_to_real:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax          ; 各レジスタをリアルモードの値で初期化
    sti

    ; --- フロッピー全域(約1.44MB)を 1MB以降(0x100000) へ読み込むループ ---
    ; 既存: mov ax, 0x1000      ; 読み込み先セグメント
    ; 既存: mov es, ax
    ; 既存: mov bx, 0           ; オフセット
    
    mov edi, 0x100000   ; Unreal Modeを利用した1MB以降の転送先ポインタ
    mov cx, 0x0002      ; CH=0(トラック), CL=2(セクタ2から開始)
    mov dx, 0x0000      ; DH=0(ヘッド), DL=0(ドライブ0)

read_loop:
    push es
    push bx
    ; BIOS読み込み用に一時バッファ(0x0000:0x8000)を設定
    xor ax, ax
    mov es, ax
    mov bx, 0x8000
    mov ah, 0x02
    mov al, 1           ; 1セクタずつ確実に読む (低速だが確実)
    int 0x13
    jc  error
    
    ; 読み込んだ512バイトを 1MB 以降へ転送
    push esi
    push ecx
    mov esi, 0x8000     ; 転送元バッファ
    mov ecx, 128        ; 512バイト / 4バイト
    db 0x66, 0xf3, 0xa5  ; rep movsd (32bit転送命令)
    pop ecx
    pop esi
    
    pop bx
    pop es
    add edi, 512        ; 1MB以降のポインタを1セクタ分進める

    ; 既存: add bx, 512         ; 1セクタ分オフセットを進める
    ; 既存: jnz next_sector     ; 64KB境界を超えていなければ次へ
    ; 既存: mov ax, es          ; 64KB超えたらセグメントを更新
    ; 既存: add ax, 0x1000
    ; 既存: mov es, ax

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
    ; 既存: cmp ch, 28          ; 何も考えずに全トラックを読んだらVRAM破壊する容量になるので、28シリンダまでにする
    cmp ch, 80          ; 1.44MB全域(80シリンダ)を読み込む
    jne read_loop

    ; --- 32bit移行 ---
    cli
    ; 既存: in al, 0x92
    ; 既存: or al, 2
    ; 既存: out 0x92, al
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
    ; 既存: mov esi, 0x10000    ; 転送元
    ; 既存: mov edi, 0x100000   ; 転送先 (1MB)
    ; 既存: mov ecx, (1007 * 512) / 4 ; 28シリンダ分(4バイト単位) --- 18 * 2 * 28 - 1
    ; 既存: rep movsd

    ; --- 転送後は元の場所(0x10000)をクリア ---
    ; 既存: mov edi, 0x10000
    ; 既存: xor eax, eax
    ; 既存: mov ecx, (1007 * 512) / 4 ; 転送したのと同じサイズ
    ; 既存: rep stosd                 ; 0で塗りつぶす

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
