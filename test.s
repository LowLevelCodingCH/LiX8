b_init:
	b 108 0

# Invalid opcode: just creates a noop, then skips over the code
iopcode:
	pop r1 0
	pop r0 0
	pop r0 0
	mov r1, 0
	mov r2, 3
# Zeroes
	dec r0 0
	dec r0 0
	dec r0 0
# Loads bad opcode into r2
	ldr r2, r0
	str r0, r1

# Pushes r2 (bad opcode)
	push r2 0
# Special interrupt, doesn't return with iret but does a cpy to pc with r0.
# Smhw iret dont work lol (smhw sub doesnt work on r0 here either)
# but hey this does work so i wont fix it until it becomes major
	cpy pc, r0

# Division by zero (e.g. 1/0) will clear all regs to 1
dbzero:
# Sets all regs to 1 so that dbzero cant happen again
# Doesnt zero the code since it is kinda important
	mov r0, 1
	mov r1, 1
	mov r2, 1
	mov r3, 1
	mov r4, 1
	mov r5, 1
	mov r6, 1
	mov r7, 1
	iret 0 0

# Double fault: when a fault doesnt exist (is 0) in the ivt and is trying to be called
dfault:
	hlt 0 0

# Protection fault: when a process tries to execute privileged instructions in user mode
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
	{iopcode:, dbzero:, dfault:,
	pfault:, syscall:, 0,
	0, 0, 0}

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
	svc 0 0
	b kernel_loop: 0

# "OS" for My emulator: LiX8 (16 bit)
