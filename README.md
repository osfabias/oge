# Osfabias Game Engine
*think about cool description*

Currently supported platforms:
- Linux;
- MacOS.

# Quick start
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

### 2. Configure and build project via CMake.
```shell
# from oge/
mkdir build
cd build
cmake .. # add -DCMAKE_BUILD_TYPE=Debug for debug build
cmake --build . -j 8
```

### 3. Start binary.
```shell
# from oge/build/
cd bin
./example
```

You're done!

# Testing
For testing OGE you need to firstly build project.
Than use this commands to start ctest.

```shell
# from oge/
cd build/tests
ctest
```

# Documentation
```shell
# from oge/
doxygen
cd doc/html
open index.html
```
