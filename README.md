## A dynamic library based raylib project
+ Uses a custom processed header file of raylib that has its own prefix (which also helps avoid name collisions with Windows.h, which is necessary for obtaining dll loading functionality)
+ Tries to be cross platform across windows and linux
+ Still need to use either of clang or gcc to compile because this project uses VLA syntax (which Microsoft refuses to implement)

## Compilation:
+ Use clang or gcc compatible compiler for both windows and linux.
+ For windows, compile without linking to any special libraries. Just that the raylib.dll must be in one of the paths that windows searches included the current directory.
+ For linux, compile with flags `-lm` and `-lraylib`. With raylib in path, the executable will have hardcoded the rpath and it will work.

How to use :
Generating maze:
Press R to reset maze at any point
There are 4 colors : skyblue, darkblue, yellow and pink
in reset phase: skyblue means potential junctions

To edit the junctions, move mouse to the cell to edit and press 0 for skyblue, 1 for darkblue

Dont add two skyblues next to each other

To generate maze , keep pressing space and it will generate incrementally

Solving maze :
Keep pressing D to solve maze , R will again reset maze


About how rand() is used to randomize:
the initial seed that this will use to make maze is not random by default
to instruct to use random seed send command line argument to the app with anything non numeric. Sending in any numeric argument to command line will cause the program to use that as seed.
