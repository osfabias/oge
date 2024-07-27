# Osfabias Game Engine
*think about cool description*

Currently supported platforms:
- MacOS.

# How to build
## Requirements:
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) (latest version)
- CMake (version 2.16)

```shell
git clone git@github.com:osfabias/oge.git
cd oge
git submodule init
git submodule update

mkdir build
cd build
cmake .. # add -DCMAKE_BUILD_TYPE=Debug for debug build
cmake --build .
```
You're done!

# Documentation
```shell
doxygen
cd doc/html
open index.html
```
