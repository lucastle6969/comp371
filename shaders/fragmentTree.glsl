#version 330 core

in vec3 pos;
uniform int color_type;
uniform int entity_position_x;
uniform int entity_position_z;

out vec4 color;

void main()
{
color = vec4(
    1.0f - abs(pos.x / 10.0f) - abs(pos.y /1000.0f),
    1.0f,
    1.0f - abs(pos.z/ 10.0f) - abs(pos.y /1000.0f),
     1.0f);
}
