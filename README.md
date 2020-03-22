# cc-tests
A temporary repository for testing Darling-CoreCrypto function implementations

## Requirements
  * C toolchain (for compiling the source files)
  * CMake (for generating build data)

Not required, but recommended: Ninja. I think it's a pretty great CMake generator.

## Usage
All of the following commands are expected to be executed in the repository root.

### Building
```bash
mkdir build
cd build
# i personally prefer the Ninja generator
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
ninja
```

### Running
```bash
./build/cctest
```
