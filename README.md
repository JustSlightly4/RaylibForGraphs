# Table of Contents
- [Introduction](#introduction)
- [Compile Instructions](#compile-instructions)
- - [Using a Single Make File](#using-a-single-make-file)
- - [Manual Make and Compilation](#manual-make-and-compilation)
<br>
<br>

# Introduction
The goal of this project is to implement a cross-platform template for a C++ project using raylib. This project expands on raylib's drawing functions by adding a new UIDrawer class that draws using a coordinate system rather than absolute pixels. This new class allows drawings, textures, and text to automatically resize when the screen changes size and hides all the math underneath the hood making for a much easier and simplier experience. This project also adds buttons to raylib making your program much more interactive.
<br>

# Compile Instructions
Compilation is a two step process that require the use of make. There are also two ways to go about compiling the project: using a single make file or the manual way that uses a make file and a compilation command.

## Using a Single Make File
As stated above, compilation is a two step process. First, a make file in the raylib directory needs to be run to compile the raylib library. Once that is complete, then the actual project needs to be compiled using g++ or another C++ compiler. I have provided a single make files that does both of these in one step. To run the make file simply run the command:

    make

To delete the .exe or .out file created by this make file run the following command:

    make clean

To delete the .exe or .out file created by this make file and delete the compiled raylib library run the following command:

    make clean-all

## Manual Make and Compilation
Once again, compilation is a two step process. First, a make file in the raylib directory needs to be run to compile the raylib library. Once that is complete, then the actual project needs to be compiled using g++ or another C++ compiler. To do this manually first run the following command:

    make -C raylib/src PLATFORM=PLATFORM_DESKTOP

This command builds raylib's library. Next, compile the project with one of the following command to produce the file output file:

    Windows: g++ -Wall -std=c++17 -I./raylib/src UIDrawer.cpp Buttons.cpp main.cpp -o main.exe ./raylib/src/libraylib.a -lopengl32 -lgdi32 -lwinmm
    
    Linux:g++ -Wall -std=c++17 -I./raylib/src UIDrawer.cpp Buttons.cpp main.cpp -o main ./raylib/src/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
<br>