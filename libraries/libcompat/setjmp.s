section .text

global setjmp
setjmp:
	mov    eax, [esp+0x4]
	mov    [eax],ebx
	mov    [eax+0x4],ebp
	mov    [eax+0x8],esi
	mov    [eax+0xc],edi
	lea    ecx,[esp+0x4]
	mov    [eax+0x10],ecx
	mov    ecx, [esp]
	mov    [eax+0x14],ecx
	xor    eax,eax
	ret

global longjmp
longjmp:
	mov    ecx, [esp+0x4]
	mov    ebx, [ecx]
	mov    ebp, [ecx+0x4]
	mov    esi, [ecx+0x8]
	mov    edi, [ecx+0xc]
	mov    eax, [esp+0x8]
	test   eax,eax
	sete   al
	mov    esp, [ecx+0x10]
	jmp    [ecx+0x14]
