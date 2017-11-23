#version 330 core

in vec3 pos;
in vec2 tex_coord;
uniform int color_type;
uniform int entity_position_x;
uniform int entity_position_z;
uniform sampler2D tex_image;

out vec4 color;

const int COLOR_WHITE = 0;
const int COLOR_COORDINATE_AXES = 1;
const int COLOR_HEIGHT = 2;
const int COLOR_TILE = 3;
const int COLOR_TEXTURE = 4;
const int COLOR_TREE = 5;

const vec4 WHITE = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main()
{
    switch (color_type) {
        case COLOR_WHITE:
            color = WHITE;
            break;
        case COLOR_COORDINATE_AXES:
            color = vec4(ceil(pos), 1.0f);
            break;
        case COLOR_HEIGHT:
            color = vec4(vec3(pos.y), 1.0f);
            break;
        case COLOR_TILE:
            color = vec4(0.5f, (entity_position_z * 32 - 1) % 256 / 256.0f, entity_position_x * 32 % 256 / 256.0f, 1.0f);
            break;
        case COLOR_TEXTURE:
            color = texture(tex_image, tex_coord);
            break;
        case COLOR_TREE:
            color = vec4(
                1.0f - abs(pos.x / 10.0f) - abs(pos.y /1000.0f),
                1.0f,
                1.0f - abs(pos.z/ 10.0f) - abs(pos.y /1000.0f),
                1.0f);
            break;
        default:
            color = WHITE;
            break;
    }
}
