[BITS 16]
[ORG 0x0000]

stage2_start:
    mov ax, cs
    mov ds, ax
    mov es, ax
    
    call enable_a20
    
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x0000
    
    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 10
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    
    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp CODE_SEG:protected_mode_start

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

[BITS 32]
protected_mode_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    
    jmp 0x10000

gdt_start:
    dq 0x0000000000000000
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

boot_drive db 0
