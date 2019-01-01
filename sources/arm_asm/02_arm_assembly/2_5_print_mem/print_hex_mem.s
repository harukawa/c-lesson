/*
  arm-none-eabi-as print_hex_mem.s -o print_hex_mem.o
  arm-none-eabi-ld print_hex_mem.o -Ttext 0x00010000 -o print_hex_mem.elf
  arm-none-eabi-objcopy print_hex_mem.elf -O binary print_hex_mem.bin
  qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_hex_mem.bin -serial mon:stdio
*/
/*
  r0 Address of message label
  r1 Address of UART 
  r2 ASCII CODE of one letter
  r3 shift number
  r13 Memory
  r14 Address of return
*/
.globl _start
_start:
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
  stmdb r13!, {r14}
  mov r3,#28
// print 0x
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
  sub r3,r3,#4
  cmp r3,#0
  bge _loop
// print \n
  mov r2,#0x0a
  bl putchar
  ldmia r13!, {r14}
  mov r15,r14
