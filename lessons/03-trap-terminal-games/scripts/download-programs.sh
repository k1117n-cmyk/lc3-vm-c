#!/bin/sh
set -eu

mkdir -p programs

curl -L -o programs/2048.obj https://www.jmeiners.com/lc3-vm/supplies/2048.obj
curl -L -o programs/rogue.obj https://www.jmeiners.com/lc3-vm/supplies/rogue.obj

echo "Downloaded programs/2048.obj and programs/rogue.obj"

