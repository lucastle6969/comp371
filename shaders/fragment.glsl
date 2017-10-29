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
            //I was just trying to get the color of the tile to change according to position,
            //so I was doing a cast: int(pos.z) , then a modulus : (int(pos.z))%256 and finaly a division by 256
            // but even without all that , you can see that the pos is not returning expected values (color not changing)
            //with pos.z and pos.z directly plugged in for green and blue values ...
            color = vec4(0.5f,pos.z, pos.x, 1.0f);
            break;

        default:
            color = WHITE;
            break;
    }
}
