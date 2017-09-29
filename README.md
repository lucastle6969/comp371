# 3d-pacman

3D PacMan game made with OpenGL. Works for macOS, Linux, Windows.

*Review Note: I'm not the most experienced with C++. General C++ style comments are welcome!*

**You may get the best results if you use CLion as your IDE - but other IDEs should work.**

## Acknowledgements

### Dependencies

Beyond the C++ standard library this application relies on:
* OpenGL
* GLFW
* GLEW
* GLM

### Other help

* Starter skeleton code adapted from [LearnOpenGL.com](https://learnopengl.com/) and COMP 371 at Concordia University
* Cross platform CMake setup created using [Hunter](https://github.com/ruslo/hunter)
* `ignoreResult` utility adapted from [StackOverflow](https://stackoverflow.com/a/30099727)
* Thanks to my friend Eric Lathrop for helping me think about camera rotation for the tilting effect

## Features

* Pacman navigates the game grid to eat dots - try to eat them all!
* Use various controls to adjust the viewing angle in three dimensions
* Use the X, Y and Z axes (red, green and blue) to keep track of things

### Controls

* Press the spacebar to move Pacman to a random new position
* Use 'U' to scale Pacman and the dots up, and 'J' to scale them down
* Move Pacman with WASD
* Rotate the game grid around its X axis using the up and down arrows
* Rotate the game grid around its Z axis using the left and right arrows
* Has the game spun out of control? Re-orient the game grid to its starting position with the 'Home' button (on some keyboards this is fn+left arrow).
* Tired of solidly painted characters? Switch to line meshes with 'L'. Switch to point clouds with 'P'. Switch back to solid triangles with 'T'.
* Dolly in and out of the scene by holding the left mouse button and moving the mouse up and down.
* Tilt the camera up and down by holding the middle mouse button and moving the mouse up and down.
* Pan, or strafe, the camera left and right by holding the right mouse button and moving the mouse left and right.

## Basic Build Requirements

* [CMake](https://cmake.org) version 3.8 or higher
      * On Windows: Make sure to check option for "add CMake to system path".
      * On Linux: If you need to build from source, [don't forget the `--system-curl` flag](https://github.com/ruslo/hunter/issues/328#issuecomment-198672048)!
* Appropriate C++ build tools for your system:
    * XCode tools on Mac
    * MinGW on Windows
    * Nothing extra to install for Linux
* Recommended IDE for all platforms: CLion (get a free 1-year student license [here](https://www.jetbrains.com/shop/eform/students))

See [Windows MinGW Setup](#markdown-header-windows-mingw-setup) below to see special instructions for installing MinGW.

See [CLion Setup](#markdown-header-clion-setup) below for help importing the project into CLion.

## Running Project in an IDE

Using Clion, you should be able to open the project and press the green play button to run the program.

*Note, dependencies will be downloaded and built from source the first time, so you will need to wait several minutes.*

For more details see [CLion Setup](#markdown-header-clion-project-setup).

## Windows MinGW Setup

1. Download and install MinGW ([click here to install](http://www.mingw.org/download/installer?)).
2. When installation is done hit continue.
3. Select `mingw32-base`, `mingw32-gcc-g++`, and `msys-base`.
4. `Installation -> Apply Changes -> Apply` to download and install the relevant packages.
5. When you see "All changes were applied successfully", hit close and exit the installation manager.

## CLion Setup

*For Windows:* Be sure to follow MinGW setup steps first.

1. If you haven't already, get a free 1-year student license for CLion [here](https://www.jetbrains.com/shop/eform/students).
2. Download and install CLion.
3. In setup, you can accept defaults.
4. Fill out other details as you like - you may want the Markdown support plugin for looking at readme files.

### CLion Project setup
1. On startup select "Open Project", and select the `3d-pacman` directory.
2. Once the project is opened, it should automatically build.
4. *Possible Windows error:* if you see a message similar to, `For MinGW to work correctly C:\Program Files\OpenSSH\bin\sh.exe must NOT be in your PATH` go to `File -> Settings -> Appearance & Behavior -> Path Variables` and add `PATH` to the Ignored Variables, hit OK, and try again.
5. Press the green play button to build and run the application.
