:
	b begin:

print:
	mov r4, 0
	mov r5, print::cursor:
	ldr r5, r5
	mov r6, vga::base:
	ldr r6, r6
	add r5, r6
	mov r6, 0
print::loop:
	ldr r6, r0
	cmp r4, r1
	biz print::end:
	str r5, r6
	inc r5
	inc r0
	inc r4
	b print::loop:
print::end:
	ret

begin:
	mov r0, string:
	mov r1, 11
	bl print:
	hlt

.data
string:
	72 101 108
	108 111 32
	87 111 114
	108 100 0

print::cursor:
	0
vga::base:
	1200
