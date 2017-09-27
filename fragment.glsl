#version 330 core

in vec3 pos;
uniform int color_type;

out vec4 color;

const int COLOR_WHITE = 0;
const int COLOR_COORDINATE_AXES = 1;
const int COLOR_YELLOW = 2;
const int COLOR_TURQUOISE = 3;

const vec4 WHITE = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 YELLOW = vec4(1.0f, 1.0f, 0.0f, 1.0f);
const vec4 ORANGE = vec4(1.0f, 0.5f, 0.2f, 1.0f);
const vec4 TURQUOISE = vec4(0.89f, 0.98f, 0.98f, 1.0f);

void main()
{
    switch (color_type) {
        case COLOR_WHITE:
            color = WHITE;
            break;
        case COLOR_COORDINATE_AXES:
            color = vec4(pos, 1.0f);
            break;
        case COLOR_YELLOW:
            color = YELLOW;
            break;
        case COLOR_TURQUOISE:
            color = TURQUOISE;
            break;
        default:
            color = ORANGE;
            break;
    }
}
