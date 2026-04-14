[bits 16]
org 0x7c00

start:
    ; 03H / extra
    ; 160x100 16 colors / VRAM = B800:0000 - B800:7FFF.
    ; 2 pixels per 2bytes.
    ; 
    ; 386以降の端末を想定しているので、VGA前提。
    ; ボードチェックしていない
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

    cli
    ; --- A20ラインを有効化 ---
    in al, 0x92
    or al, 2
    out 0x92, al

    ; --- Unreal Mode 移行処理 ---
    lgdt [gdtr]
    mov eax, cr0
    or  al, 1
    mov cr0, eax        ; プロテクトモードへ一時移行
    jmp pmode_entry
pmode_entry:
    mov bx, 0x10        ; データセグメント記述子(0x10)を選択
    mov fs, bx          ; fs のリミットを 4GB に拡張 (BIOSはfsを原則破壊しない)
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax        ; リアルモードへ復帰
    jmp 0:back_to_real
back_to_real:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax          ; 各レジスタをリアルモードの値(0)で初期化
    sti

    ; --- フロッピー全域(約1.44MB)を 0x10000 へ読み込む ---
    mov edi, 0x100000   ; 1MB以降の転送先ポインタ
    mov cx, 0x0002      ; CH=0(トラック), CL=2(セクタ2から開始)
    mov dx, 0x0000      ; DH=0(ヘッド), DL=0(ドライブ0)

read_loop:
    ; BIOS読み込み用に一時バッファ(0x0000:0x8000)を設定
    push es
    push bx
    xor ax, ax
    mov es, ax
    mov bx, 0x8000
    mov ah, 0x02
    mov al, 1           ; 1セクタずつ確実に読む (低速だが確実)
    int 0x13
    jc  error

    ; Unreal Mode (fs) を利用して 1MB 以降へ 512バイト転送
    push esi
    push ecx
    mov esi, 0x8000     ; 転送元バッファ
    mov ecx, 128        ; 512バイト / 4バイト
.copy_sector:
    mov eax, [ds:esi]
    mov [fs:edi], eax   ; fs(4GBリミット)を使用して 1MB超えへ書き込み
    add esi, 4
    add edi, 4
    loop .copy_sector
    pop ecx
    pop esi

    pop bx
    pop es

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
    cmp ch, 80          ; フロッピー全域(160トラック：両面読み80回)を読み込む
    jne read_loop

    ; --- 32bit移行 ---
    cli
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
