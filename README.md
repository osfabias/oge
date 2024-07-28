# Osfabias Game Engine
*think about cool description*

Currently supported platforms:
- MacOS.

# How to build
Requirements:
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) (latest version)
- CMake (version 2.16+)
- GLSLC

### 1. Clone project and submodules.
```shell
git clone git@github.com:osfabias/oge.git
cd oge
git submodule init
git submodule update
```

### 2. Configure project via CMake.
```shell
# from oge/
mkdir build
cd build
cmake .. # add -DCMAKE_BUILD_TYPE=Debug for debug build
cmake --build .
```

### 3. Start binary.
```shell
# from oge/build/
cd bin
./example
```

You're done!

# Documentation
```shell
doxygen
cd doc/html
open index.html
```
