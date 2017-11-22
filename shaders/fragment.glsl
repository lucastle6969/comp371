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

uniform vec3 sunPosition; // test point light variable
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
//some values for point light
float quadratic = 0.032;
float linear = 0.1;
float constant = 1;

struct DirLight{
    vec3 direction;
};

struct PointLight {
    vec3 position;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight;

vec3 calculateColor(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDirection);
vec3 calculateColor (DirLight light, vec3 normal, vec3 viewDirection);

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
            vec3 pixelColor = calculateColor(pointLight, norm, worldPos, viewDir); // add second color for sun


            color = vec4(pixelColor,1.0);

            break;
        }
        default:
            color = WHITE;
            break;
    }
}

vec3 calculateColor(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDirection)
{
    vec3 lighDirection = normalize(light.position - fragpos);
    float diffuse = max(dot(normal, lighDirection),0.0);
    vec3 reflecDirection = reflect(-lighDirection, normal);
    float specular = pow(max(dot(viewDirection, reflecDirection), 0.0), material.shininess);
    float distance = length(light.position - fragpos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic *(distance * distance));
    vec3 ambientValue;
    vec3 diffuseValue;
    vec3 specularValue;

    if ( textureSize(tex_image,0).x > 0){
        ambientValue = material.ambient * attenuation;
        diffuseValue = material.diffuse * diffuse * attenuation;
        specularValue = material.specular * specular * attenuation;
    } else {
        ambientValue = material.ambient * vec3(texture(tex_image, tex_coord)) * attenuation;
        diffuseValue = material.diffuse * diffuse * vec3(texture(tex_image, tex_coord)) * attenuation;
        specularValue = material.specular * specular * vec3(texture(tex_image, tex_coord)) * attenuation;
    }
    return (ambientValue + diffuseValue + specularValue);
}

vec3 calculateColor (DirLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-light.direction);
    float diffuse = max(dot(normal, lightDirection),0.0);
    vec3  reflecDirection = reflect(-lightDirection, normal);
    float specular = pow(max(dot(viewDirection, reflecDirection),0.0), material.shininess);
    vec3 ambientValue;
    vec3 diffuseValue;
    vec3 specularValue;

    if ( textureSize(tex_image,0).x > 0){
        ambientValue = material.ambient;
        diffuseValue = material.diffuse * diffuse;
        specularValue = material.specular * specular;
    } else {
        ambientValue = material.ambient * vec3(texture(tex_image, tex_coord));
        diffuseValue = material.diffuse * diffuse * vec3(texture(tex_image, tex_coord));
        specularValue = material.specular * specular * vec3(texture(tex_image, tex_coord));
    }
    return (ambientValue + diffuseValue + specularValue);
}
