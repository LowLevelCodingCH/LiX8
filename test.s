b 42 0
:isr0
	mov r0, 0
	iret 0 0
:isr1
	mov r0, 1
	iret 0 0
:isr2
	mov r0, 2
	iret 0 0
:isr3
	mov sp, 0
	hlt 0 0
	iret 0 0
:ivt
	:isr0 :isr1 :isr2
	:isr3 0 0
:begin
	swi 1 0
	in 0, 0
:end
	svcstr :ivt 0
	b :begin 0
