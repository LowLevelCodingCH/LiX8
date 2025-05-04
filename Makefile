COMP = g++
ARGS = -o
C = $(COMP) $(ARGS)

buildhw:
	$(C) lix16 main.cpp
	$(C) lxsm  asm/simple.cpp
	./lxsm helloworld.s -Pc


build:
	$(C) lix16 main.cpp
	$(C) lxsm  asm/simple.cpp
	./lxsm os.s -Pc

testhw: buildhw
	./lix16

test: build
	./lix16

clean: build
	rm lix16
	rm lxsm
	rm a.bin
