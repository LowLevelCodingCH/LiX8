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
	bl set::regs:
	cpy sp, r0
	svcstr :
	adrum :
	adrbs :
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
	mov r0, kbd::mmio:
	ldr r0, r0
	ldr r0, r0
	bl putchar:
	iret
svc::syscall:
	adrum kas:
	adrbs kas:
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
ivt:
	excep::invopc:, excep::dbzero:,
	excep::dfault:, excep::pfault:,
	hwint::kbint:,
	svc::syscall:,
scalltest:
	mov r0, 1
	mov r1, finddat:
	mov r2, 3
	svc 5
	ret
finddat:
	65, 69, 105
vga::base:
	24576
print::cursor:
	0
ide::dat:
	26032
ide::adr:
	26033
ide::cmd:
	26034
ide::buf:
	26035
kbd::mmio:
	25776
kas:
	0
ksp:
	8192
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

	adrum uas:
	adrbs uas:
	
	mov sp, usp:
	ldr sp, sp
	
	swi 1

	mov r0 1 mov r1 finddat: mov r2 3 svc 5

	hlt
