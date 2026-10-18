CC ?= cc
CFLAGS ?= -Wall -Wextra -pedantic -std=c99
VM := ../lc3
LC3AS ?= lc3as

.PHONY: assemble assemble-mini assemble-printf run run-mini run-printf inspect inspect-mini inspect-printf compare compare-printf clean

assemble-mini: build/hello-minias.obj

build:
	mkdir -p build

build/minias: minias.c | build
	$(CC) $(CFLAGS) minias.c -o build/minias

build/hello-minias.obj: src/hello.asm build/minias | build
	./build/minias src/hello.asm build/hello-minias.obj

run-mini: build/hello-minias.obj $(VM)
	$(VM) build/hello-minias.obj

inspect-mini: build/hello-minias.obj
	xxd build/hello-minias.obj

assemble-printf: build/hello-printf.obj

build/hello-printf.obj: | build
	printf '\x30\x00\xe0\x02\xf0\x22\xf0\x25\x00\x48\x00\x65\x00\x6c\x00\x6c\x00\x6f\x00\x2c\x00\x20\x00\x57\x00\x6f\x00\x72\x00\x6c\x00\x64\x00\x21\x00\x0a\x00\x00' > build/hello-printf.obj

run-printf: build/hello-printf.obj $(VM)
	$(VM) build/hello-printf.obj

inspect-printf: build/hello-printf.obj
	xxd build/hello-printf.obj

compare-printf: build/hello-printf.obj build/hello-minias.obj
	cmp -s build/hello-printf.obj build/hello-minias.obj
	xxd build/hello-minias.obj

assemble: build/hello.obj

build/hello.obj: src/hello.asm | build
	$(LC3AS) src/hello.asm
	mv src/hello.obj build/hello.obj
	mv src/hello.sym build/hello.sym

run: build/hello.obj $(VM)
	$(VM) build/hello.obj

inspect: build/hello.obj
	xxd build/hello.obj

compare: build/hello.obj build/hello-minias.obj
	cmp -s build/hello.obj build/hello-minias.obj
	xxd build/hello-minias.obj

clean:
	rm -f build/hello.obj build/hello.sym build/hello-minias.obj build/hello-printf.obj build/minias src/hello.obj src/hello.sym
