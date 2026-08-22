# Lesson 12 Verification

Run these commands from `lessons/12-release`.

## Build

```sh
make
```

Expected result:

```text
./lc3
```

## Minimal HALT Program

`programs/*.obj` files are ignored, so create the smallest test object locally:

```sh
mkdir -p programs
printf '\x30\x00\xf0\x25' > programs/halt.obj
./lc3 programs/halt.obj
```

Expected output:

```text
HALT
```

## Loop Program

```sh
./lc3 examples/loop.obj
```

Expected output:

```text
Hi
Hi
Hi
HALT
```

## Profile

```sh
./lc3 --profile examples/loop.obj
```

Expected output:

```text
Hi
Hi
Hi
HALT

profile:
total instructions: 15

BR              3
ADD             4
AND             1
LEA             3
TRAP            4
```

## Memory Dump

```sh
./lc3 --dump-memory x3000 12 examples/loop.obj
```

Expected output:

```text
Hi
Hi
Hi
HALT

memory x3000..x300B:
x3000: 5260 1263 E004 F022 127F 03FC F025 0048
x3008: 0069 000A 0000 0000
```

## Cleanup

```sh
make clean
rm -f programs/halt.obj
```
