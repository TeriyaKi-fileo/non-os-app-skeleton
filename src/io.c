#include "io.h"

void ioOut8(int port, int data) {
    __asm__ __volatile__ ("outb %%al, %%dx" : : "d" (port), "a" (data));
}

unsigned char ioIn8(int port) {
    unsigned char data;
    __asm__ __volatile__ ("inb %%dx, %%al" : "=a" (data) : "d" (port));
    return data;
}

static inline void ioVsyncWait(void) {
    __asm__ __volatile__ (
        "movw $0x3da, %%dx\n\t"
        
        // 1. すでにV-Blank期間中なら、終わるまで待機
        "wait_end:\n\t"
        "inb %%dx, %%al\n\t"
        "testb $0x08, %%al\n\t"
        "jnz wait_end\n\t"
        
        // 2. V-Blankが始まる（新フレームの開始）まで待機
        "wait_start:\n\t"
        "inb %%dx, %%al\n\t"
        "testb $0x08, %%al\n\t"
        "jz wait_start\n\t"
        : 
        : 
        : "ax", "dx" // 使用するレジスタを破壊対象として通知
    );
}

void ioHlt(void) {
    __asm__ __volatile__ ("hlt");
}

void ioCli(void) {
    __asm__ __volatile__ ("cli");
}

void ioSti(void) {
    __asm__ __volatile__ ("sti");
}
