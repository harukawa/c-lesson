.globl _start
_start:
  ldr r13,=0x08000000
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
/*
  r0  Address of message label
  r1  Address of UART 
  r3  ASCII CODE of one letter
  r13 Memory
  r14 Address of return
*/
putchar:
  // TODO: fix here!
  ldr r1,=0x101f1000
  str r3, [r1]
  mov r15, r14

print:
  // TODO: Fix this function too.
  stmdb r13!, {r14}
  ldrb r3,[r0]
_loop:  
  bl putchar
  add r0, r0, #1
  ldrb r3,[r0]
  cmp r3,#0
  bne _loop
  ldmia r13!, {r14}
  mov r15, r14

msg1: .asciz "First text.\n"
msg2: .asciz "Second text!\n"
