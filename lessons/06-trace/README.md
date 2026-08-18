# Lesson 06: Trace

Fixed source snapshot for:

```text
Cで作るLC-3仮想マシン 第6回: 命令実行をトレースしてVMの動きを見る
```

## Build

```sh
make
```

## Run

Normal execution:

```sh
./lc3 examples/hello.obj
```

Trace execution:

```sh
./lc3 --trace examples/hello.obj
```

## Files

```text
lc3.c
Makefile
examples/hello.asm
examples/hello.obj
expected-output/normal.txt
expected-output/trace.txt
```

