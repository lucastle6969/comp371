
#version 330 core

in vec3 v_position;
in vec3 n_position;
in vec2 tex_position;

out vec3 FragPos;
out vec3 Normal;
out vec2 texture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    FragPos = vec3(model * vec4(v_position, 1.0));
    Normal = mat3(transpose(inverse(model))) * n_position;
    texture = tex_position;

    gl_Position = projection * view * vec4(FragPos, 1.0);

}