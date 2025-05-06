:
	b init:

# Arg0 = r0 str
# Arg1 = r1 len
# Arg2 = r2
# Arg3 = r3
# Arg4-ArgN = stack
print:
# Counter
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

ivt:
# Exceptions
	0, 0, 0, 0,
# Syscall
	0,

vga::base:
	1200
print::cursor:
	0

# k adr space
kas:
	0
# k stck ptr
ksp:
	8192
# same but for user mode
usp:
	16384
uas:
	16384

init:
	svcstr ivt:
	adrum kas:
	adrbs kas:
	mov sp, ksp:
	ldr sp, sp

	hlt
