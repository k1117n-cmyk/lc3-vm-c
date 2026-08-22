# Lesson 11: Debug Example

This is the workspace for the eleventh article.

It uses the VM features from lesson 10 to debug a small LC-3 loop program.

Article:

https://pc-fan.net/lc3-vm-debug-example/

## Build

```sh
make
```

## Example Program

`examples/hello.asm` is the previous small Hello World sample.

`examples/loop.asm` is the lesson 11 sample. It prints `Hi` three times.

```asm
        AND R1, R1, #0
        ADD R1, R1, #3
LOOP    LEA R0, MSG
        PUTS
        ADD R1, R1, #-1
        BRp LOOP
        HALT
MSG     .STRINGZ "Hi\n"
```

The assembled object file is included as `examples/loop.obj`.

## Run

```sh
./lc3 examples/loop.obj
./lc3 --trace examples/loop.obj
./lc3 --profile examples/loop.obj
./lc3 --break x3002 examples/loop.obj
./lc3 --dump-registers examples/loop.obj
./lc3 --dump-memory x3000 16 examples/loop.obj
```

The debugging options can be combined:

```sh
./lc3 --trace --dump-registers examples/loop.obj
./lc3 --profile --dump-memory x3000 12 examples/loop.obj
./lc3 --break x3005 --dump-registers examples/loop.obj
```
