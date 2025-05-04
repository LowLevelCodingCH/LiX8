# : exists because then you can just do
# b : #0
# to restart.
# Yes, lxsm (my assembler), can do that.
# It is because of my laziness though (not to exclude the : when parsing symbols)
:

# NOTE: add logging to all exceptions

# Branches to init_sys: (bootstrapper)
b_init:
	b init_sys: #0

# Standard functions
# Arg N: rN
print:
# Null terminated
	mov r7, #0
# Cursor so we dont overwrite previously written strings
	mov r2, print_cursor:
# MMIO adr for printing
	mov r3, #1200
	ldr r2, r0
	add r2, r3
print_loop:
	ldr r3, r0
	cmp r3, r7
	biz print_end: #0
	str r2, r3

	inc r0 #0
	inc r2 #0
	b print_loop: #0
print_end:
	mov r3, #1200
	sub r2, r3
	dec r3 #0
	str r3, r2
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
	adrum #0 #0
	mov r1, #3
	sub sp, r1

	ldr r0, sp
	sub r0, r1
	add sp, r1
	mov r1, #0
# Zeroes
	str r0, r1
	iret #0 #0
	adrum #16384 #0

# Division by zero (e.g. 1/0) will clear all regs to 1
dbzero:
# Sets all regs to 1 so that dbzero cant happen again
# Doesnt zero the code since it is kinda important
	adrum #0 #0
	mov r0, #1
	mov r1, #1
	mov r2, #1
	mov r3, #1
	mov r4, #1
	mov r5, #1
	mov r6, #1
	mov r7, #1
	iret #0 #0
	adrum #16384 #0

# Double fault: when a fault doesnt exist (is 0) in the ivt and is trying to be called
# Should halt.
dfault:
	adrbs #0 #0
	adrum #0 #0
	adrum #16384 #0
	hlt #0 #0

# Protection fault: when a process tries to execute privileged instructions in user mode
pfault:
	adrbs #0 #0
	adrum #0 #0
# Prints a message
	mov r0, pfault_msg:
	bl print: #0
	adrum #16384 #0
	iret #0 #0

syscall:
	adrbs #0 #0
	adrum #0 #0
# Syscall code: coming soon
	adrum #16384 #0
	iret #0 #0

# Initializes the operating system
init_sys:
init_kadrspce:
# So see this is a funny quirk: if the usermode address space is not 0 and we are in kernel mode
# and we try to write somewhere below the usermode adr space we get a pfault.
# even in kernel mode. because im too lazy to check if were in kernel mode and then allow writes everywhere
# so what we do is set it to 0 to be unable to write below it. to effectively make this fault impossible to reach
# until we change the adr space
# And adrbs sets the base address which gets added to the address of each str/ldr inst
	adrbs #0 #0
	adrum #0 #0
init_stck:
# Initializes LR (interrupt vector (leap register)) and SP
# Stack grows upward (+), instead of downward (-) like in intel
# Because i find the downwards thingy to be confusing af when dealing w/ lowlevel stuff
	mov sp, #8192
	svcstr ivt: #0

init_regs:
# Initializes registers
	bl clregs: #0

print_init:
	mov r0, inited_str:
	bl print: #0

finalize:
	bl clregs: #0

init_umode:
# Sets the usermode address
	adrbs #0 #0
	adrum #16384 #0

# Switches to user mode (SWItch)
# Yes, we do have a mode bit and yes, the impl works
	swi #1 #0

b_loop:
	b kernel_loop: #0

# At this point i need to talk to "HDDs" etc. to load programs from disk to memory

# Will likely be a scheduler or sth
kernel_loop:
	b kernel_loop: #0
	hlt #0 #0


# Padding because we dont want writes to the "data" section
# (cpu doesnt rlly care. it doesnt see this as data but as noop noop noop noop...)
# to interfere with code. and vice versa

sec_data_pad:
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
	#0 #0 #0 #0 #0 #0
# Faults with invalid opcode if code execution gets here (should never happen)
	#69 #420 #6969
# Then halts
	hlt hlt hlt

# Slots 0-3 are faults. invalid opcode, division by zero, double fault, protection fault
# you can have up to 65536 interrupts. because that is the max addressing space.
# Of course 65536 - OFFSET OF IVT.
# if we have 32 bit well get
# (about) 4BILLION - OFFSET OF IVT
# and this scales.
ivt:
	iopcode:, dbzero:, dfault:,
	pfault:, syscall:, #0,
	#0, #0, #0

# Statically declared variable which defines the cursor for the print subroutine
# So if we print twice we dont overwrite the previously written string
print_cursor:
	#0

# Status strings

pfault_msg:
	#80 #114 #111
	#116 #101 #99
	#116 #105 #111
	#110 #32 #70
	#97 #117 #108
	#116 #10 #0

inited_str:
	#73 #110 #105
	#116 #105 #97
	#108 #105 #122
	#101 #100 #10
	#0 #0 #0

unused_vars:
# Only 2 because print_cursor takes 1 already
	#0 #0
	#0 #0 #0

# "OS" for My emulator: LiX8 (16 bit) (my own architecture)
# Instructions are always 3 words (16 bits * 3 (48 bits)) long because i want it to be simple
# here you just count the lines and then multiply by 3
# In intel you gotta watch for prefixes, addresses of different sizes etc.
