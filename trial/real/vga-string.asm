        org     0x7c00
        [bits 16]
        ; vga buffer
        mov     ax, 0xb800
        mov     es, ax
        ; clear screan
        mov     si, 0
        mov     ah, 0x07
        mov     al, ' '
clear:
        mov     [es:si], ax
        add     si, 2
        cmp     si, 80*2
        jl      clear
        ; print string
        mov     si, 0
        mov     di, 0
        mov     ah, 0x07
str:
        mov     al, [string + di]
        mov     [es:si], ax
        inc     di
        add     si, 2
        cmp     al, 0
        jne     str

        ; string def
        string  db "hello world!", 0
        jmp     $
        times   510-($-$$) db 0
        dw      0xAA55