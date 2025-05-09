scalltest:
# Print: 1
	mov r0, 1
# String: (e.g.) finddat
	mov r1, finddat:
# Length: (e.g.) 3
	mov r2, 3
# Execute the call
	svc 5

	ret
