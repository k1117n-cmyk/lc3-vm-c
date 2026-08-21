# Lesson 10: Dump

This is the fixed code for the tenth article.

It adds final-state dump options for registers and memory.

## Build

```sh
make
```

## Run

```sh
./lc3 --dump-registers examples/hello.obj
./lc3 --dump-memory x3000 16 examples/hello.obj
```

The debugging options can be combined:

```sh
./lc3 --trace --dump-registers examples/hello.obj
./lc3 --profile --dump-memory x3000 16 examples/hello.obj
./lc3 --break x3002 --dump-registers examples/hello.obj
```
