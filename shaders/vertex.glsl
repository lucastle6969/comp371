
#version 330 core
  
in vec3 v_position;
in vec2 tex_coord_in;

uniform mat4 light_mvp_matrix;
uniform mat4 mvp_matrix;
uniform mat4 light_bias_mvp;

uniform bool isShadowMapping;

out vec3 pos;
out vec2 tex_coord;

void main()
{
    
    if(isShadowMapping){
        
        gl_Position =  light_mvp_matrix * vec4(v_position,1);
        return;
    }
    
    
    gl_Position = mvp_matrix * vec4(v_position.x, v_position.y, v_position.z, 1.0);
    pos = v_position;
    tex_coord = tex_coord_in;


}
