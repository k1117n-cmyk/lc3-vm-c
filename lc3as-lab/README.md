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

## Compare Direct Bytes With lc3as

To verify the earlier Hello World article, compare the `.obj` written directly
with `printf` (`build/hello-printf.obj`) against the `.obj` produced by `lc3as`
(`build/hello.obj`):

```sh
make compare-printf
```

If `lc3as` is not on your `PATH`, specify its location:

```sh
make LC3AS=/path/to/lc3as compare-printf
```

The command exits successfully only when the two files match byte for byte,
then displays the `printf` version with `xxd`. The output begins with
`3000 e002 f022 f025`.

On macOS, a downloaded `lc3as` executable may be blocked with an "Apple cannot
check it for malicious software" warning. If you trust the `lc3tools` package,
run `make lc3as` in its directory and pass the path to the newly built
executable using `LC3AS`.

## Compare The Tiny Assembler With lc3as

To compare the tiny assembler output (`build/hello-minias.obj`) with the
`lc3as` output (`build/hello.obj`), run:

```sh
make compare
```

If `lc3as` is installed somewhere else, pass the path:

```sh
make compare LC3AS=/path/to/lc3as
```
