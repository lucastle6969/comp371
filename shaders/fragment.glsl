#version 330 core

in vec3 pos;
uniform int color_type;

out vec4 color;

const int COLOR_WHITE = 0;
const int COLOR_COORDINATE_AXES = 1;
const int COLOR_HEIGHT = 2;
const int COLOR_TILE = 3;

const vec4 WHITE = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main()
{
    switch (color_type) {
        case COLOR_WHITE:
            color = WHITE;
            break;
        case COLOR_COORDINATE_AXES:
            color = vec4(pos, 1.0f);
            break;
        case COLOR_HEIGHT:
            color = vec4(vec3(pos.y), 1.0f);
            break;
        case COLOR_TILE:

            color = vec4(0.5f,float((int(pos.z))%256)/256.0, float((int(pos.x))%256)/256.0, 1.0f);
            break;

        default:
            color = WHITE;
            break;
    }
}
