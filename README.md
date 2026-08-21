# LC-3 VM in C

[![Language: C](https://img.shields.io/badge/Language-C-blue)](https://en.wikipedia.org/wiki/C_(programming_language))
![Topic: LC-3 VM](https://img.shields.io/badge/Topic-LC--3_VM-lightgrey)
![Platform: UNIX](https://img.shields.io/badge/Platform-UNIX-informational)
![Purpose: Learning](https://img.shields.io/badge/Purpose-Learning-green)

LC-3 educational computer virtual machine written in C.

Repository:

https://github.com/k1117n-cmyk/lc3-vm-c

This repository is a companion project for the UNIX Cafe blog series:

- [Cで作るLC-3仮想マシン 第1回: 16bit CPUの骨格を作る](https://pc-fan.net/lc3-vm-skeleton/)
- [Cで作るLC-3仮想マシン 第2回: 命令セットを実装する](https://pc-fan.net/lc3-vm-instruction-set/)
- [Cで作るLC-3仮想マシン 第3回: TRAPと端末入力でゲームを動かす](https://pc-fan.net/lc3-vm-trap-terminal-games/)
- [Cで作るLC-3仮想マシン 第4回: LC-3アセンブリでHello Worldを書く](https://pc-fan.net/lc3-assembly-hello-world/)
- [Cで作るLC-3仮想マシン 第5回: 小さいLC-3アセンブラを作る](https://pc-fan.net/tiny-lc3-assembler/)
- [Cで作るLC-3仮想マシン 第6回: 命令実行をトレースしてVMの動きを見る](https://pc-fan.net/lc3-vm-trace/)
- [Cで作るLC-3仮想マシン 第7回: 命令ごとの実行回数をプロファイルする](https://pc-fan.net/lc3-vm-profile/)
- [Cで作るLC-3仮想マシン 第8回: ステップ実行で1命令ずつ進める](https://pc-fan.net/lc3-vm-step/)
- [Cで作るLC-3仮想マシン 第9回: ブレークポイントで指定番地で止める](https://pc-fan.net/lc3-vm-c-breakpoint/)
- [Cで作るLC-3仮想マシン 第10回: レジスタとメモリをダンプして状態を見る](https://pc-fan.net/lc3-vm-c-dump-registers-memory/)

The VM loads LC-3 `.obj` files and executes them from memory. It can run the sample `2048.obj` and `rogue.obj` programs from the original LC-3 VM tutorial.
The `lc3as-lab/` directory also includes a tiny assembler that can assemble the
Hello World example used in the series.

## Files

```text
.
├── lc3.c
├── Makefile
├── README.md
├── LICENSE
├── ATTRIBUTION.md
├── lc3as-lab/
│   ├── minias.c
│   ├── Makefile
│   ├── README.md
│   └── src/
│       └── hello.asm
├── lessons/
│   ├── 01-skeleton/
│   ├── 02-instruction-set/
│   ├── 03-trap-terminal-games/
│   ├── 06-trace/
│   ├── 07-profile/
│   ├── 08-step/
│   ├── 09-breakpoint/
│   ├── 10-dump/
│   ├── 11-debug-example/
│   └── 12-release/
├── programs/
│   └── .gitkeep
└── scripts/
    └── download-programs.sh
```

## Requirements

- macOS or Linux
- C compiler such as `cc`, `clang`, or `gcc`
- `make`
- `curl` if you want to download the sample programs

This implementation uses UNIX APIs such as `termios` and `select()`.

## Build

```sh
make
```

This creates the executable:

```text
./lc3
```

## Download Sample Programs

The sample `.obj` files are not included in this repository. Download them from the original tutorial supplies:

```sh
./scripts/download-programs.sh
```

This creates:

```text
programs/2048.obj
programs/rogue.obj
```

You can also download them manually:

```sh
mkdir -p programs
curl -L -o programs/2048.obj https://www.jmeiners.com/lc3-vm/supplies/2048.obj
curl -L -o programs/rogue.obj https://www.jmeiners.com/lc3-vm/supplies/rogue.obj
```

## Run

Run the VM with an LC-3 object file:

```sh
./lc3 programs/2048.obj
```

or:

```sh
./lc3 programs/rogue.obj
```

Trace execution:

```sh
./lc3 --trace lessons/06-trace/examples/hello.obj
```

For 2048, answer `y` when asked:

```text
Are you on an ANSI terminal (y/n)?
```

Controls:

```text
W  up
A  left
S  down
D  right
```

## Tiny Assembler

Build the VM first, then run the tiny assembler lab:

```sh
make
cd lc3as-lab
make run-mini
```

This assembles `src/hello.asm` with `minias.c` and runs the generated `.obj`
with the VM.

Expected output:

```text
Hello, World!
HALT
```

## Inspect an Object File

LC-3 `.obj` files are binary files. Use `xxd` to inspect them:

```sh
xxd programs/2048.obj | head
```

The first 16-bit word is the origin address. For example:

```text
3000
```

means the program is loaded into LC-3 memory starting at address `x3000`.

## Troubleshooting

If your terminal input looks broken after stopping the VM, reset the terminal:

```sh
reset
```

If the VM aborts immediately, check:

- the `.obj` file path is correct
- endian conversion is implemented
- all required opcodes are implemented
- PC-relative offsets are calculated from the already-incremented PC

## References

- Write your Own Virtual Machine: https://www.jmeiners.com/lc3-vm/
- Original GitHub repository: https://github.com/justinmeiners/lc3-vm
- LC-3 ISA reference: https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf
