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
