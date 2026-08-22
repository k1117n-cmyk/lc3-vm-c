# Lesson 12: Release

This is the final release-oriented snapshot for the LC-3 VM article series.

It mirrors the public repository shape used in the last article: a root VM
source file, a simple `Makefile`, a README-oriented project layout, sample
program helpers, fixed examples, license information, and verification notes.

## Contents

```text
.
├── lc3.c
├── Makefile
├── README.md
├── VERIFY.md
├── LICENSE
├── ATTRIBUTION.md
├── .gitignore
├── examples/
│   ├── hello.asm
│   ├── hello.obj
│   ├── loop.asm
│   └── loop.obj
├── programs/
│   └── .gitkeep
└── scripts/
    └── download-programs.sh
```

## Build

```sh
make
```

This creates:

```text
./lc3
```

## Included Examples

`examples/hello.obj` is the small Hello World object file used earlier in the
series.

`examples/loop.obj` is the lesson 11 loop program. It prints `Hi` three times
and is useful for checking trace, profile, break, and dump output.

## Run

```sh
./lc3 examples/loop.obj
./lc3 --trace examples/loop.obj
./lc3 --profile examples/loop.obj
./lc3 --dump-memory x3000 12 examples/loop.obj
```

## External Programs

The original 2048 and Rogue object files are not committed here. Download them
when needed:

```sh
./scripts/download-programs.sh
./lc3 programs/2048.obj
./lc3 programs/rogue.obj
```

See `ATTRIBUTION.md` for the source of those sample programs.

## Verification

See `VERIFY.md` for the exact commands and expected output used by the final
article.
