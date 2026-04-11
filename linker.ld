OUTPUT_FORMAT("binary")

SECTIONS
{
    /* カーネルの開始アドレス */
    . = 0x100000;

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
        *(.rodata)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}
