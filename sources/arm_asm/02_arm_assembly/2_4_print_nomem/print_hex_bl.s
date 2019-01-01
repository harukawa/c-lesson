/*
  arm-none-eabi-as print_hex_bl.s -o print_hex_bl.o
  arm-none-eabi-ld print_hex_bl.o -Ttext 0x00010000 -o print_hex_bl.elf
  arm-none-eabi-objcopy print_hex_bl.elf -O binary print_hex_bl.bin
  qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_hex_bl.bin -serial mon:stdio
*/
/*
  r0 Address of message label
  r1 Address of UART 
  r2 ASCII CODE of one letter
  r3 shift number
*/
.globl _start
_start:
  ldr r1,=0x101f1000
  
  mov r0, r15 
  bl print_hex
  mov r0, #0x68
  bl print_hex
  b end

end:
  b end

print_hex:
  mov r3,#28
  mov r2,#0x30
  str r2,[r1]
  mov r2,#0x78
  str r2,[r1]
print_hex_loop:
  lsr r2,r0,r3
  and r2,r2,#0x0f
  cmp r2,#0x0a
  blt number
  add r2,r2,#0x27

number:
  add r2,r2,#0x30
  str r2,[r1]
  sub r3,r3,#4
  cmp r3,#0
  blt loop_end
  b print_hex_loop

loop_end:
  mov r2,#0x0a
  str r2,[r1]
  mov r15,r14
