COMP = g++
ARGS = -o
C = $(COMP) $(ARGS)

build:
	$(C) lix16   main.cpp
	$(C) lxsm    asm/simple.cpp

# I always am root, i use my system as root 24/7
install: build
	mv lxsm /usr/bin/lxsm
	mv lix16 /usr/bin/lix16

clean: build
	rm lix16
	rm lxsm
