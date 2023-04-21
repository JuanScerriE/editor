#!/bin/sh

scripts/compile_shaders.sh && cmake -S . -B build -GNinja && scripts/copy_compile_commands.sh && ninja -C build
