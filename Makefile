COMP = g++
ARGS = -o
C = $(COMP) $(ARGS)

buildpc:
	$(C) lix16 main.cpp
	$(C) lxsm  asm/simple.cpp
	$(C) readlxe lxe/readlxe.cpp
	./lxsm os.s -Pc

build:
	$(C) lix16   main.cpp
	$(C) lxsm    asm/simple.cpp
	$(C) readlxe lxe/readlxe.cpp
	./lxsm os.s

testpc: buildpc
	./lix16

test: build
	./lix16

clean: build
	rm lix16
	rm lxsm
	rm a.bin
	rm readlxe
