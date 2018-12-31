/*
 arm-none-eabi-as print_hex.s -o print_hex.o
 arm-none-eabi-ld print_hex.o -Ttext 0x00010000 -o print_hex.elf
 arm-none-eabi-objcopy print_hex.elf -O binary print_hex.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_hex.bin -serial mon:stdio
*/
.globl _start
_start:
    ldr r0,=0x101f1000
    ldr r1,=0xdea9beaf
	mov r3,#28
    b print_hex
print_hex:
    lsr r2,r1,r3
	and r2,r2,#0x0f
	cmp r2,#0x0a
	blt number
	add r2,r2,#0x27
number:
	add r2,r2,#0x30
	str r2,[r0]
	sub r3,r3,#4
	cmp r3,#0
	blt end
	b print_hex
	
end:
    b end

