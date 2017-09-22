# gl-obj-skeleton

CMake Skeleton code for OpenGL. Works for macOS, Linux, Windows.

**You will get the best results if you use CLion as your IDE.**

Original files provided by COMP 371 at Concordia.

Cross platform CMake setup created using [Hunter](https://github.com/ruslo/hunter).

## Basic Requirements

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

## Usage

Using Clion, you should be able to open the project and press the green play button to run the program.

*Note, dependencies will be downloaded and built from source the first time, so you will need to wait several minutes.*

For more details see [CLion Setup](#markdown-header-clion-project-setup).

## Expected Output

If the starter program runs as expected you should see something like:

![OpenGL Program Screenshot](triangle_program.jpg)

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
1. On startup select "Open Project", and select the `gl-obj-skeleton` directory.
2. Once the project is opened, it should automatically build.
4. *Possible Windows error:* if you see a message similar to, `For MinGW to work correctly C:\Program Files\OpenSSH\bin\sh.exe must NOT be in your PATH` go to `File -> Settings -> Appearance & Behavior -> Path Variables` and add `PATH` to the Ignored Variables, hit OK, and try again.
5. Press the green play button to build and run the application.
