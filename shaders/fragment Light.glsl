#version 330 core

out vec4 FragColor;

struct DirLight{

    vec3 direction;
    vec3 ambient;

};

struct PointLight {

    vec3 position;
    vec3 ambient;

};

struct Material {

    vec3 color;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D image;

};


uniform vec3 viewPos;
uniform vec3 color; //color of object
//uniform vec3 position;
uniform vec3 direction;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 ligthSpecular;
uniform float shininess;
uniform sampler2D image; // image for texture

in vec3 FragPos;  
in vec3 Normal;  
in vec2 texture;

void main()
{




    vec3 pixelColor = color;

    //vec3 pixelColor = color * texture(image, texture).rgb;


    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(direction);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //calculate pixel color
    vec3 ambient = lightAmbient * pixelColor;
    vec3 diffuse = lightDiffuse * diff * pixelColor;
    vec3 specular = ligthSpecular * spec;
    FragColor =  vec4((ambient + diffuse + specular),0.0);


}
