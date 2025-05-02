b_init:
	b 45 0

# All exceptions (EXCEPT prot fault) halt the system
exc:
	hlt 0 0

pfault:
	mov r0, 0
	mov r1, 0
	mov r2, 0
	mov r3, 0
	mov r4, 0
	mov r5, 0
	mov r6, 0
	mov r7, 0
	iret 0 0

syscall:
	iret 0 0

ivt:
	exc:, exc:, exc:,
	pfault:, exc:, exc:,
	syscall:, exc:, exc:

# Initializes the operating system
init_sys:
init_regs:
# Initializes registers
	mov r0, 0
	mov r1, 0
	mov r2, 0
	mov r3, 0
	mov r4, 0
	mov r5, 0
	mov r6, 0
	mov r7, 0

init_stck:
# Initializes LR (interrupt vector (leap register)) and SP
	mov sp, 8192
	svcstr ivt: 0

init_umode:
# Switches to user mode
	swi 1 0

# At this point i need to talk to "HDDs" etc. to load programs from disk to memory

# Will likely be a scheduler or sth
kernel_loop:
	b kernel_loop: 0
