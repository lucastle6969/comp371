#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D shadowmap;


in vec2 UV;

void main(){
    
    float depthValue= texture(shadowmap, UV).r;
    color = vec4 (vec3(depthValue),1.0);
}
