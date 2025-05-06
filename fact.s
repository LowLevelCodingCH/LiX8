# iterative factorial

b begn:

fact:
# i = 1
	cpy r1, r0
	mov r2, 1
	mov r3, 1
fact::loop:
	cmp r1, r2
	biz fact::end:

	mul r0, r3

	dec r1
	inc r3

	b fact::loop:
fact::end:
	ret


# fact(3) = 3*2*1 = 6
# fact(5) = 5*4*3*2*1 = 120

begn:
	mov r0, fact_num:
	ldr r0, r0
	bl fact:
	hlt

.data

fact_num:
	5
