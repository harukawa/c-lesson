ldr r1, [r15, #0x34]
mov r0, #0x68
str r0, [r1]
mov r0,#0x65
str r0,[r1]
mov r0,#0x6c
str r0,[r1]
str r0,[r1]
mov r0,#0x6f
str r0,[r1]
mov r0,#0x0D
str r0,[r1]
mov r0,#0x0A
str r0,[r1]
loop:
b loop
.raw 0x101f1000