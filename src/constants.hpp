#ifndef PROCEDURALWORLD_CONSTANTS_H
#define PROCEDURALWORLD_CONSTANTS_H

#include <string>

static const int COLOR_WHITE = 0;
static const int COLOR_COORDINATE_AXES = 1;
static const int COLOR_HEIGHT = 2;
static const int COLOR_TILE = 3;

static const char* APP_NAME = "Procedural World";

// Window dimensions
static const GLuint WIDTH = 800, HEIGHT = 800;

// bounds for placing grid, axes and objects
static const int WORLD_X_MIN = -10;
static const int WORLD_X_MAX = 10;
static const int WORLD_Y_MIN = -10;
static const int WORLD_Y_MAX = 10;
// this one is only used for the z-axis
static const int WORLD_Z_MAX = 10;

static const float PLAYER_MOVEMENT_SPEED = 0.01;

#endif // PROCEDURALWORLD_CONSTANTS_H
