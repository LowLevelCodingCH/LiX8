b_init:
	b init_sys: #0

# Standard functions
# Arg N: rN
print:
# Null terminated
	mov r7, #0
# MMIO adr for printing
	mov r2, #1200
print_loop:
	ldr r3, r0
	cmp r3, r7
	biz print_end: #0
	str r2, r3

	inc r0 #0
	inc r2 #0
	b print_loop: #0
print_end:
	ret #0 #0

clregs:
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	mov r6, #0
	mov r7, #0
	ret #0 #0

# Invalid opcode: just creates a noop, then skips over the code
iopcode:
	mov r1, #3
	sub sp, r1

	ldr r0, sp
	sub r0, r1
	add sp, r1
	mov r1, #0
# Zeroes
	str r0, r1
	iret #0 #0

# Division by zero (e.g. 1/0) will clear all regs to 1
dbzero:
# Sets all regs to 1 so that dbzero cant happen again
# Doesnt zero the code since it is kinda important
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	mov r6, #0
	mov r7, #0
	iret #0 #0

# Double fault: when a fault doesnt exist (is 0) in the ivt and is trying to be called
dfault:
	hlt #0 #0

# Protection fault: when a process tries to execute privileged instructions in user mode
pfault:
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	mov r6, #0
	mov r7, #0
	iret #0 #0

syscall:
	iret #0 #0

# Initializes the operating system
init_sys:
init_stck:
# Initializes LR (interrupt vector (leap register)) and SP
	mov sp, #8192
	svcstr ivt: #0

init_regs:
# Initializes registers
	bl clregs: #0

init_umode:
# Switches to user mode
	swi #1 #0

print_init:
	mov r0, inited_str:
	bl print: #0

finalize:
	bl clregs: #0

b_loop:
	b kernel_loop: #0

ivt:
	iopcode:, dbzero:, dfault:,
	pfault:, syscall:, #0,
	#0, #0, #0

# Status strings
inited_str:
	#73 #110 #105
	#116 #105 #97
	#108 #105 #122
	#101 #100 #10
	#0 #0 #0

# At this point i need to talk to "HDDs" etc. to load programs from disk to memory

# Will likely be a scheduler or sth
kernel_loop:
# Tests invalid opcode
#	b kernel_loop: #0
	hlt #0 #0

# "OS" for My emulator: LiX8 (16 bit)
