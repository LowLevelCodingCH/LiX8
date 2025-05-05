:
	b init:

ivt:
# Exceptions
	0, 0, 0, 0,
# Syscall
	0,

# k adr space
kas:
	0
# k stck ptr
ksp:
	8192
# same but for user mode
usp:
	16384
uas:
	16384

init:
	svcstr ivt:

