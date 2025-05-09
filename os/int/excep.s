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
