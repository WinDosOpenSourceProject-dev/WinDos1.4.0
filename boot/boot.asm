[BITS 16]
[ORG 0x7C00]

%define KERNEL_LOAD_SEGMENT 0x1000
%define STAGE2_LOAD_SEGMENT 0x2000

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov ax, 0x0003
    int 0x10

    mov si, msg_welcome
    call print_string

    mov ax, STAGE2_LOAD_SEGMENT
    mov es, ax
    mov bx, 0x0000
    
    mov ah, 0x02
    mov al, 8
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    
    jc disk_error
    
    jmp STAGE2_LOAD_SEGMENT:0x0000

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

boot_drive db 0
msg_welcome db 0x0D, 0x0A, "WinDos 4.0 Bootloader", 0x0D, 0x0A, "Loading System...", 0x0D, 0x0A, 0
msg_disk_error db "Disk Error! Press any key...", 0

times 510-($-$$) db 0
dw 0xAA55
