# Lesson 07: Profile

Fixed source snapshot for:

```text
Cで作るLC-3仮想マシン 第7回: 命令ごとの実行回数をプロファイルする
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

Profile execution:

```sh
./lc3 --profile examples/hello.obj
```

## Files

```text
lc3.c
Makefile
examples/hello.asm
examples/hello.obj
expected-output/normal.txt
expected-output/trace.txt
expected-output/profile.txt
```
