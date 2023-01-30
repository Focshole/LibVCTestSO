#!/bin/sh
cd externalLibrary/testlib
cargo build --release
cargo install --force cbindgen
cbindgen --crate testlib --output ../HowToImport.h