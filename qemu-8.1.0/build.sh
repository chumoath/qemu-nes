#!/bin/sh

rm -rf build
mkdir build && cd build

../configure               \
--enable-debug             \
--enable-debug-tcg         \
--target-list=nes-softmmu


make -j6
