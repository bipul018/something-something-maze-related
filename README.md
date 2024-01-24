Uses raylib and custom processed header file of raylib for avoiding possible name collisions with Windows.h

So obviously only works in windows

But also it won't work with MSVC as it uses VLA syntax, so need to use clang or gcc to compile this


In this first version to use, 
just press space and it will step by step generate maze
press r to reset 

warning: the initial seed that this will use to make maze is not random by default
to instruct to use random seed send command line argument to the app with anything non numeric
