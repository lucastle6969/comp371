#version 330 core

in vec3 pos;
in vec3 worldPos;
in vec3 worldNormal;
in vec2 tex_coord;

uniform int color_type;
uniform int entity_position_x;
uniform int entity_position_z;
uniform sampler2D tex_image;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform float shininess;

// uniform vec3 sunPosition; // test point light variable
uniform vec3 sunVector;
uniform vec3 worldViewPos;

out vec4 color;

const int COLOR_WHITE = 0;
const int COLOR_COORDINATE_AXES = 1;
const int COLOR_HEIGHT = 2;
const int COLOR_TILE = 3;
const int COLOR_TEXTURE = 4;
const int COLOR_LIGHTING = 5;

const vec4 WHITE = vec4(1.0f, 1.0f, 1.0f, 1.0f);

struct DirLight{
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D image;
};

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
        case COLOR_LIGHTING: {
            // inspired by tutorial at: https://learnopengl.com/#!Lighting/Basic-Lighting

            // properties
            vec3 norm = normalize(worldNormal);
            vec3 viewDir = normalize(worldViewPos - worldPos);

            vec3 lightDir = normalize(sunVector);
            float diffuseShading = max(dot(norm, lightDir), 0.0);
            vec3 reflectDir = reflect(-lightDir, norm);
            float specularShading = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            // calculate pixel color
            vec3 diffuse = lightDiffuse * diffuseShading;
            vec3 specular = lightSpecular * specularShading;

            color = vec4((lightAmbient + diffuse + specular), 0.0);
            break;
        }
        default:
            color = WHITE;
            break;
    }
}
