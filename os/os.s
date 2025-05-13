#import !init.s
#import !funcs.s
#import !int/excep.s
#import !int/hwint.s
#import !int/syscall.s
#import !int/ivt.s
#import !scalltest.s
#import !vars.s

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

# Userspace here

	hlt
