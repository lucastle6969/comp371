
#version 330 core
  
in vec3 vPosition;

uniform mat4 mvp_matrix;

void main()
{
    gl_Position = mvp_matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
}
