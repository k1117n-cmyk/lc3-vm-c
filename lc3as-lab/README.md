# Tiny LC-3 Assembler Lab

This directory contains a small LC-3 assembler used by the UNIX Cafe blog
series. It intentionally supports only the instructions and directives needed
for the Hello World example.

## Files

```text
lc3as-lab/
├── minias.c
├── Makefile
├── README.md
└── src/
    └── hello.asm
```

## Scope

`minias.c` supports:

- `.ORIG`
- `.END`
- `.STRINGZ`
- `LEA`
- `PUTS`
- `HALT`

It uses two passes:

1. Pass 1 records label addresses.
2. Pass 2 encodes instructions and directives into LC-3 words.

The generated `.obj` file is written in big-endian byte order.

## Build And Run

From the repository root:

```sh
make
cd lc3as-lab
make run-mini
```

Expected output:

```text
Hello, World!
HALT
```

To inspect the generated object file:

```sh
make inspect-mini
```

The first words should be:

```text
3000 e002 f022 f025
```

Meaning:

- `3000`: origin
- `e002`: `LEA R0, HELLO`
- `f022`: `PUTS`
- `f025`: `HALT`

## Compare With The Direct Bytes

You can compare the tiny assembler output with the direct byte sequence used in
the earlier Hello World article:

```sh
make compare-printf
```

## Optional: Compare With lc3as

If you have an `lc3as` command installed, you can compare against it:

```sh
make compare
```

If `lc3as` is installed somewhere else, pass the path:

```sh
make compare LC3AS=/path/to/lc3as
```
