b begin: #0

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

begin:
	mov r0, hworld_str:
	bl print: #0
	hlt #0 #0

hworld_str:
	#72 #101 #108
	#108 #111 #32
	#87 #111 #114
	#108 #100 #0
