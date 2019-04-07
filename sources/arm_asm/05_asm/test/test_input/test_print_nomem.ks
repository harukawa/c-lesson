ldr r1,=msg1
bl print
ldr r1,=msg2
bl print
end:
b end
print:
ldrb r3,[r1]
ldr r0, [r15, #0x30]
loop:
str r3,[r0]
add r1, r1, #0x1
ldrb r3,[r1]
cmp r3,#0x0
bne loop
mov r15, r14
msg1:
.raw "First text.\n"
msg2:
.raw "Second text!\n"
.raw 0x101f1000
