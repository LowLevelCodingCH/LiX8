b test: #0

# r1 = result
fact:
# r4 = i
# r3 = temp
# r0 = N
mov r3, #1
mov r4, #1
.loop:
# fact *= i
	mul r3, r4
	inc r4 #0
# if (i <= N)
	cmp r0, r4
	biz .loop: #0
	bim .loop: #0
# If none of the conditions are met the function is done and we return
	cpy r1, r3
	ret #0 #0


test:
	mov r0, #5
	bl fact: #0
	hlt #0 #0
