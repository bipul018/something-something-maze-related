Uses raylib and custom processed header file of raylib for avoiding possible name collisions with Windows.h

So obviously only works in windows

But also it won't work with MSVC as it uses VLA syntax, so need to use clang or gcc to compile this
