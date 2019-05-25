setjmp:
   mov ecx, 4[esp]
   mov 0[ecx], ebp
   mov 4[ecx], esi
   mov 8[ecx], edi
   mov 12[ecx], ebx
   mov eax, 0[esp]
   mov 16[ecx], eax
   xor eax, eax
   ret

longjmp:
   mov ecx, 4[esp]
   mov ebp, 0[ecx]
   mov esi, 4[ecx]
   mov edi, 8[ecx]
   mov ebx, 12[ecx]
   mov 8[esp], eax
   jmp 16[ecx]16