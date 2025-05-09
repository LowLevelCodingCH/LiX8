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
