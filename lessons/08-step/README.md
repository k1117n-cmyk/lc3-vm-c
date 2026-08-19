# Lesson 08: Step Execution

This is the fixed code for the eighth article.

It adds a `--step` option. In step mode, the VM prints trace output and waits
for Enter after each instruction, so the execution of a small LC-3 program can
be followed one instruction at a time.

## Build

```sh
make
```

## Run

```sh
./lc3 --step examples/hello.obj
```

Press Enter at each `step>` prompt to execute the next instruction.

Expected flow:

```text
PC=3000 INSTR=E002 OP=LEA
R0=3003 R1=0000 R2=0000 R3=0000 R4=0000 R5=0000 R6=0000 R7=0000 PC=3001 COND=P

step>
PC=3001 INSTR=F022 OP=TRAP PUTS
Hello, World!
R0=3003 R1=0000 R2=0000 R3=0000 R4=0000 R5=0000 R6=0000 R7=3002 PC=3002 COND=P

step>
PC=3002 INSTR=F025 OP=TRAP HALT
HALT
R0=3003 R1=0000 R2=0000 R3=0000 R4=0000 R5=0000 R6=0000 R7=3003 PC=3003 COND=P
```
