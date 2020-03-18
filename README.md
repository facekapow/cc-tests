# cc-tests
A temporary repository for testing Darling-CoreCrypto function implementations

## Requirements
  * C toolchain (for compiling the source files)
  * CMake (for generating build data)
  * Node.js (for generating test data)
  * Python (for generating test expectations)

Not required, but recommended: Ninja. I think it's a pretty great CMake generator.

## Usage
All of the following commands are expected to be executed in the repository root.

### Generating Test Data
```bash
# this will use Python as well to generate test expectations
node scripts/generate-data.js
```

### Building
```bash
mkdir build
cd build
# i personally prefer the Ninja generator
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
ninja
```

### Running Tests
```bash
./build/cctest
```
