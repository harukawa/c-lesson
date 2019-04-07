  ldr r13,=0x08000000
  mov r0, r15 
  bl print_hex
  mov r0, #0x68
  bl print_hex
end:
  b end
putchar:
  ldr r1,=0x101f1000
  str r2, [r1]
  mov r15, r14  
print_hex:
  stmdb r13!, {r1-r3,r14}
  mov r3,#0x28
  mov r2,#0x30
  bl putchar
  mov r2,#0x78
  bl putchar
_loop:
  lsr r2,r0,r3
  and r2,r2,#0x0f
  cmp r2,#0x0a
  blt _number
  add r2,r2,#0x27
_number:
  add r2,r2,#0x30
  bl putchar
  sub r3,r3,#0x4
  cmp r3,#0x0
  bge _loop
  mov r2,#0x0a
  bl putchar
  ldmia r13!, {r1-r3,r14}
  mov r15,r14