global gdtUpdate
gdtUpdate:
  lgdt [rdi]
  mov ax, 0x10
  mov ss, ax
  mov ds, ax
  mov es, ax
  mov rax, qword .trampoline
  push qword 0x8
  push rax
  o64 retf
.trampoline:
  ret

global tssUpdate
tssUpdate:
  mov ax, 0x28
  ltr ax
  ret
