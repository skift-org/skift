;; --- interrupt request ---------------------------------------------------- ;;

extern irq_handler

%macro IRQ_NAME 1
dd irq%1
%endmacro

%macro IRQ 1
irq%1:
    cli
    push 0
    push %1
    jmp irq_common
%endmacro

irq_common:
    pushad

    push ds
    push es
    push fs
    push gs 

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    cld

    mov eax, esp
    push esp

    call irq_handler

    mov esp, eax

    pop gs
    pop fs
    pop es
    pop ds

    popad
    
    add esp, 8 ; pop errcode and int number
    
    iret

IRQ 0 
IRQ 1 
IRQ 2 
IRQ 3 
IRQ 4 
IRQ 5 
IRQ 6 
IRQ 7 
IRQ 8 
IRQ 9 
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

global irq_vector
irq_vector:
    IRQ_NAME 0
    IRQ_NAME 1
    IRQ_NAME 2
    IRQ_NAME 3
    IRQ_NAME 4
    IRQ_NAME 5
    IRQ_NAME 6
    IRQ_NAME 7
    IRQ_NAME 8
    IRQ_NAME 9
    IRQ_NAME 10
    IRQ_NAME 11
    IRQ_NAME 12
    IRQ_NAME 13
    IRQ_NAME 14
    IRQ_NAME 15

;; --- Interrupts Service Routine ------------------------------------------- ;;

extern isr_handler

%macro ISR_NAME 1
dd __isr%1
%endmacro

%macro ISR_ERR 1
__isr%1:
    cli
    push %1
    jmp isr_common
%endmacro

%macro ISR_NOERR 1
__isr%1:
    cli
    push 0
    push %1
    jmp isr_common
%endmacro

isr_common:
    pushad

    push ds
    push es
    push fs
    push gs 

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop gs
    pop fs
    pop es
    pop ds

    popad
    
    add esp, 8 ; pop errcode and int number
    
    iret

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

ISR_NOERR 128


global isr_vector
isr_vector:
    ISR_NAME 0
    ISR_NAME 1
    ISR_NAME 2
    ISR_NAME 3
    ISR_NAME 4
    ISR_NAME 5
    ISR_NAME 6
    ISR_NAME 7
    ISR_NAME 8
    ISR_NAME 9
    ISR_NAME 10
    ISR_NAME 11
    ISR_NAME 12
    ISR_NAME 13
    ISR_NAME 14
    ISR_NAME 15
    ISR_NAME 16
    ISR_NAME 17
    ISR_NAME 18
    ISR_NAME 19
    ISR_NAME 20
    ISR_NAME 21
    ISR_NAME 22
    ISR_NAME 23
    ISR_NAME 24
    ISR_NAME 25
    ISR_NAME 26
    ISR_NAME 27
    ISR_NAME 28
    ISR_NAME 29
    ISR_NAME 30
    ISR_NAME 31
    
    ISR_NAME 128