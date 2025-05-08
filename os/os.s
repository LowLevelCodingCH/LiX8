:
	b init:

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
	cpy sp, r0
# svcstr doesnt work on registers
	svcstr :
# this dereferences, so adrbs and adrum (sX where X is a number) will be the opcode of "b", 18
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

excep::invopc:
	adrum kas:
	adrbs kas:

	mov r1, 4
	sub sp, r1
	ldr r0, sp
	add sp, r1
	mov r1, 1
	sub r0, r1
	mov r1, 0
	str r0, r1

	adrum uas:
	adrbs uas:
	iret

excep::dbzero:
	adrum kas:
	adrbs kas:

	mov r0, 1
	bl set::regs:

	adrum uas:
	adrbs uas:
	iret


excep::dfault:
	adrum kas:
	adrbs kas:

	svcstr ivt:

	adrum uas:
	adrbs uas:
	iret

excep::pfault:
	adrum kas:
	adrbs kas:

	adrum uas:
	adrbs uas:
	iret

hwint::kbint:
#	adrum kas:
#	adrbs kas:
#
	mov r0, kbd::mmio:
	ldr r0, r0
	ldr r0, r0
	bl putchar:
#
#	adrum uas:
#	adrbs uas:
	iret

# R0 = syscall num
# R1 = arg1
# ...... - R4
svc::syscall:
	adrum kas:
	adrbs kas:

# Syscall to compare with
	mov r5, 0
	cmp r0, r5
	biz svc::syscall::end:

	mov r5, 1
	cmp r0, r5
	biz svc::syscall::conprint:

svc::syscall::end:
	adrum uas:
	adrbs uas:
	iret

svc::syscall::conprint:
	cpy r0, r1
	cpy r1, r2
	bl print:
	b svc::syscall::end:

init:
	svcstr ivt:
	adrum kas:
	adrbs kas:

	mov sp, ksp:
	ldr sp, sp

	adrum uas:
	adrbs uas:
	
	mov sp, usp:
	ldr sp, sp
	
	swi 1


	hlt

finddat:
	65, 69, 105
# Aie

ivt:
# Exceptions
	excep::invopc:, excep::dbzero:,
	excep::dfault:, excep::pfault:,
	hwint::kbint:,
# Syscall
	svc::syscall:,

vga::base:
	24576
print::cursor:
	0

# IDE mmio

ide::dat:
	26032
ide::adr:
	26033
ide::cmd:
	26034
ide::buf:
	26035

# KBD mmio

kbd::mmio:
	25776

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
