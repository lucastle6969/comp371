#ifndef PROCEDURALWORLD_CONSTANTS_H
#define PROCEDURALWORLD_CONSTANTS_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <string>

static const int COLOR_WHITE = 0;
static const int COLOR_COORDINATE_AXES = 1;
static const int COLOR_HEIGHT = 2;
static const int COLOR_TILE = 3;
static const int COLOR_UNLIT_TEXTURE = 4;
static const int COLOR_LIGHTING = 5;
static const int COLOR_TREE = 6;
static const int COLOR_SKY_TEXTURE = 7;
static const int COLOR_FONT = 8;

static unsigned int FONT_STYLE_MYTHOS = 1;
static unsigned int FONT_STYLE_OUTLINE = 2;

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

static const float PLAYER_MOVEMENT_SPEED = 0.2f;

static const float WATER_ELEVATION = -0.04f;

static const float HORIZONTAL_TERRAIN_SCALE = 3.0f;
static const float VERTICAL_TERRAIN_SCALE = 0.4f;

static const GLint DEFAULT_MINIFICATION_FILTER = GL_NEAREST;
static const GLint DEFAULT_MAGNIFICATION_FILTER = GL_NEAREST;

static const GLint DEFAULT_TEXTURE_WRAP = GL_MIRRORED_REPEAT;

static const bool DEFAULT_STB_VERT_ALIGN_FLIP = false;

#endif // PROCEDURALWORLD_CONSTANTS_H
