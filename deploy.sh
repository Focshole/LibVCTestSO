#!/bin/bash
rm -rf build/
rm -rf cmake/
mkdir cmake
cp libVC/config/FindLibVersioningCompiler.cmake cmake/
cmake -S . -B build
cmake --build build
