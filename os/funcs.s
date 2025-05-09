putchar:
	mov r5, print::cursor:
	mov r6, vga::base:
	ldr r5, r5
	ldr r6, r6
	add r5, r6
	str r5, r0
	sub r5, r6
	inc r5
	mov r6, print::cursor:
	str r6, r5

	ret

set::allregs:
	bl set::regs:
	cpy sp, r0
# svcstr doesnt work on registers
	svcstr :
# this dereferences, so adrbs and adrum (sX where X is a number) will be the opcode of "b", 18
# Since the ":" label's first (and only) instruction is b init:
	adrum :
	adrbs :
# Yes ret doesnt work, yes we leave a return adr on the stack.
# but this is set::allregs:
	cpy pc, r0
	ret

set::regs:
	cpy r0, r0
	cpy r1, r0
	cpy r2, r0
	cpy r3, r0
	cpy r4, r0
	cpy r5, r0
	cpy r6, r0
	cpy r7, r0
	cpy r8, r0
	cpy r9, r0
	cpy r10, r0
	cpy r11, r0
	cpy r12, r0
	cpy r13, r0
	cpy r14, r0
	cpy r15, r0
	ret

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
