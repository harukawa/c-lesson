ldr r13,=0x08000000
ldr r0,=msg1
bl print
ldr r0,=msg2
bl print
end:
b end
putchar:
ldr r1,=0x101f1000
str r3, [r1]
mov r15, r14
print:
stmdb r13!, {r1,r3,r14}
ldrb r3,[r0]
_loop:  
bl putchar
add r0, r0, #0x1
ldrb r3,[r0]
cmp r3,#0x0
bne _loop
ldmia r13!, {r1,r3,r14}
mov r15, r14
msg1:
.raw "First text.\n"
msg2:
.raw "Second text!\n"