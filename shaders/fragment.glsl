#version 330 core

in vec3 pos;
in vec3 worldPos;
in vec3 worldNormal;
in vec2 tex_coord;

uniform int color_type;
uniform int entity_position_x;
uniform int entity_position_z;
uniform sampler2D tex_image;

uniform vec3 object_ambient;
uniform vec3 object_diffuse;
uniform vec3 object_specular;
uniform float object_shininess;

uniform vec3 light_color;
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
    float object_shininess;
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

            vec3 norm = normalize(worldNormal);
            vec3 viewDir = normalize(worldViewPos - worldPos);
            // we're passing in the direction of the sun but we want the vector
            // pointing TOWARD the sun!
            vec3 lightDir = -1.0f * normalize(sunVector);
            vec3 reflectDir = reflect(-lightDir, norm);

            // compute diffuse and specular shading
            float diffuseShading = max(dot(norm, lightDir), 0.0);
            float specularShading = pow(max(dot(viewDir, reflectDir), 0.0), object_shininess);

            // calculate ambient, diffuse and specular color
            vec3 ambient = object_ambient * light_color;
            vec3 diffuse = object_diffuse * diffuseShading * light_color;
            vec3 specular = object_specular * specularShading * light_color;

            // add up components
            color = vec4((ambient + diffuse + specular), 0.0);
            break;
        }
        default:
            color = WHITE;
            break;
    }
}
