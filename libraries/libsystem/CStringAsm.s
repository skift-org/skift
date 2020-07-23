global memcpy
memcpy:
    push ebp
    mov ebp, esp

    ; Save used registers
    push edx
    push ecx
    push edi
    push esi

    ; Size
    mov ecx, [ebp + 0x10]
    ; If the size is zero, return
    test ecx, ecx
    jz .done
    
    ; Source pointer 
    mov esi, [ebp + 0x0c]

    ; Destination pointer
    mov edi, [ebp + 0x08]

    ; Check if esi and edi are valid
    mov eax, edi
    or eax, esi
    jz .done

    ; The returned value is the destination pointer
    mov eax, edi

    ; Check if the size is a tiny value
    cmp ecx, 8
    jle .small

    ; If not, copy normally
    jmp .copy

.small:
    cmp ecx, 1
    je .n1
    cmp ecx, 2
    je .n2
    cmp ecx, 3
    je .n3
    cmp ecx, 4
    je .n4
    cmp ecx, 5
    je .n5
    cmp ecx, 6
    je .n6
    cmp ecx, 7
    je .n7
    cmp ecx, 8
    je .n8

.n1: 
    mov dl, [esi]
    mov [edi], dl
    jmp .done

.n2: 
    mov dx, [esi]
    mov [edi], dx
    jmp .done

.n3: 
    mov dx, [esi]
    mov [edi], dx
    mov dl, [esi + 2]
    mov [edi + 2], dl
    jmp .done

.n4: 
    mov edx, [esi]
    mov [edi], edx
    jmp .done

.n5: 
    mov edx, [esi]
    mov [edi], edx
    mov dl, [esi + 4]
    mov [edi + 4], dl
    jmp .done

.n6: 
    mov edx, [esi]
    mov [edi], edx
    mov dx, [esi + 4]
    mov [edi + 4], dx
    jmp .done

.n7:
    mov edx, [esi]
    mov [edi], edx
    mov dx, [esi + 4]
    mov [edi + 4], dx
    mov dl, [esi + 6]
    mov [edi + 6], dl
    jmp .done

.n8:
    mov edx, [esi]
    mov [edi], edx
    mov edx, [esi + 4]
    mov [edi + 4], edx
    jmp .done

.copy:
    mov edx, ecx
    shr ecx, 2
    rep movsd
    mov ecx, edx
    and ecx, 3

    ; Copy the remaining bytes
.padding:
    test ecx, ecx
    jz .done
    mov dl, [esi]
    mov [edi], dl
    inc esi
    inc edi
    dec ecx
    jmp .padding

.done:
    ; Restore the registers we fucked up :-)
    pop esi
    pop edi
    pop ecx
    pop edx

    pop ebp
    ret
