# Lesson 09: Breakpoint

This is the fixed code for the ninth article.

It adds a `--break xADDR` option. The VM runs normally until `PC` reaches the
specified LC-3 address, then prints the current instruction and registers before
continuing.

## Build

```sh
make
```

## Run

```sh
./lc3 --break x3002 examples/hello.obj
```

Press Enter at the `break>` prompt to continue.

Expected flow:

```text
Hello, World!
breakpoint hit at x3002
PC=3002 INSTR=F025 OP=TRAP HALT
R0=3003 R1=0000 R2=0000 R3=0000 R4=0000 R5=0000 R6=0000 R7=3002 PC=3002 COND=P

break>
HALT
```

The debugging options can be combined:

```sh
./lc3 --trace --break x3002 examples/hello.obj
./lc3 --profile --break x3002 examples/hello.obj
./lc3 --step --break x3002 examples/hello.obj
```
