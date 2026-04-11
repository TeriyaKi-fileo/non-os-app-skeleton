#include "io.h"

void ioOut8(int port, int data) {
    __asm__ __volatile__ ("outb %%al, %%dx" : : "d" (port), "a" (data));
}

unsigned char ioIn8(int port) {
    unsigned char data;
    __asm__ __volatile__ ("inb %%dx, %%al" : "=a" (data) : "d" (port));
    return data;
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
