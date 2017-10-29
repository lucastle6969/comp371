# comp-371-procedural-generation-project

Loads a raster image and renders a 3D surface by mapping brightness at a given location to surface height at the corresponding location.

Works on macOS and Linux. Should work for Windows, but not recently tested.

## Features

* Follow a character around an on-the-fly generated world!

### Controls

* Use WASD or the arrow keys to move the player forward, back, left or right
* Click the window with the left mouse button to capture the mouse and enter camera look mode
* Move the mouse in camera look mode to rotate about the player and change the forward direction
* Press escape to exit camera look mode, and get your mouse back
* Press the grave/backtick button (`) to toggle the visibility of the X, Y and Z axes (hidden by default)
* Press backspace to reset the camera

## Building

See [Building and running the project](docs/building.md).

## Acknowledgements

### Dependencies

Beyond the C++ standard library this application relies on:
* OpenGL
* GLFW
* GLEW
* GLM
* STB (particularly the stb_image.h header library)

(Following standard setup procedure in a CMake-compatible IDE should fetch all these dependencies for you, so you shouldn't need to download them separately.)

### Other help

* Starter skeleton code adapted from [LearnOpenGL.com](https://learnopengl.com/) (including inspiration for [camera direction movement](https://learnopengl.com/#!Getting-started/Camera) and COMP 371 at Concordia University
* Cross platform CMake setup created using [Hunter](https://github.com/ruslo/hunter)
* Camera direction mouse movement partially modeled after tutorial
* Thanks to the COMP 371 TAs and my classmates for helping me think through some of these tougher problems
