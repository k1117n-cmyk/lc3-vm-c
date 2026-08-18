# Lesson 03: TRAP and Terminal Games

Fixed source snapshot for:

```text
Cで作るLC-3仮想マシン 第3回: TRAPと端末入力でゲームを動かす
```

This is the original VM state before the later trace, profile, step, breakpoint, and dump features are added.

## Build

```sh
make
```

## Download Sample Programs

The game `.obj` files are not stored in this repository. Download them with:

```sh
./scripts/download-programs.sh
```

This creates:

```text
programs/2048.obj
programs/rogue.obj
```

## Run

```sh
./lc3 programs/2048.obj
./lc3 programs/rogue.obj
```

For 2048, answer `y` when asked whether you are on an ANSI terminal.

## Files

```text
lc3.c
Makefile
programs/
scripts/download-programs.sh
```

