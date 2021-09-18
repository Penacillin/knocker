# Knocker

A lightweight implementation of [libgourou](http://indefero.soutade.fr/p/libgourou/) utils.
Avoids use of QT.

# Build

## Requirements
```bash
cmake
libcurl4-openssl-dev
libssl-dev
zlib1g-dev
libzip-dev
```

## Build
If you have python invoke available
```bash
inv build -c
```
with just CMake:
```bash
cmake --configure  --build build
cmake --build build --target all
```

## Usage
```bash
# To 'activate' a device (creates .xml and devicesalt files needed for acsmdownloader)
./build/bin/adeptactivate -u "username" -p "password"
# Download epub from acsm
./build/bin/acsmdownloader -f book.acsm
```