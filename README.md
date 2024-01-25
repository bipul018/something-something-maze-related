Uses raylib and custom processed header file of raylib for avoiding possible name collisions with Windows.h

So obviously only works in windows

But also it won't work with MSVC as it uses VLA syntax, so need to use clang or gcc to compile this


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
to instruct to use random seed send command line argument to the app with anything non numeric
