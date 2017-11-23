#version 330 core

in vec3 pos;
in vec3 worldPos;
in vec3 worldNormal;
in vec2 tex_coord;

struct SunLight {
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
};

uniform SunLight sunLight;
uniform PointLight pointLight;
uniform Material material;

uniform int color_type;
uniform int entity_position_x;
uniform int entity_position_z;
uniform sampler2D tex_image;

uniform bool use_texture;

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

struct ColorComponents {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

ColorComponents calculateColor(
    const vec3 light_dir,
    const vec3 light_color,
    const vec3 normal,
    const vec3 view_dir
);

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

            vec3 normal = normalize(worldNormal);
            vec3 view_dir = normalize(worldViewPos - worldPos);
            // we're passing in the direction of the sunlight but we want the vector
            // pointing TOWARD the sun!
            vec3 sun_dir = -1.0f * normalize(sunLight.direction);
            vec3 pointlight_dir = normalize(pointLight.position - worldPos);
            float point_distance = length(pointLight.position - worldPos);
            float attenuation = 1.0f /
                (constant + linear * point_distance + quadratic * (point_distance * point_distance));

            // get individual components (before texture/attenuation)
            // of sunlight and point light
            ColorComponents sunlight_components = calculateColor(
                sun_dir,
                sunLight.color,
                normal,
                view_dir
            );
            ColorComponents pointlight_components = calculateColor(
                pointlight_dir,
                pointLight.color,
                normal,
                view_dir
            );

            vec3 ambientValue =
                max(sunlight_components.ambient, 0) +
                max(pointlight_components.ambient * attenuation, 0);
            vec3 diffuseValue =
                max(sunlight_components.diffuse, 0) +
                max(pointlight_components.diffuse * attenuation, 0);
            vec3 specularValue =
                max(sunlight_components.specular, 0) +
                max(pointlight_components.specular * attenuation, 0);

            if (use_texture) {
                // multiply components against texture value but only if we've
                // got a texture!
                vec3 tex3 = vec3(texture(tex_image, tex_coord));
                ambientValue *= tex3;
                diffuseValue *= tex3;
                specularValue *= tex3;
            }

            color = vec4((ambientValue + diffuseValue + specularValue), 0.0);
            break;
        }
        default:
            color = WHITE;
            break;
    }
}

/*
vec3 calculateColor(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDirection)
{
    vec3 lighDirection = normalize(fragpos - light.position);
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
    //return (ambientValue + diffuseValue);
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
*/
// compute the basic color without considering attenuation or texture multiplication
ColorComponents calculateColor(
    const vec3 light_dir,
    const vec3 light_color,
    const vec3 normal,
    const vec3 view_dir
) {
    vec3 reflect_dir = reflect(-light_dir, normal);

    // compute diffuse and specular shading
    float diffuse_shading = max(dot(normal, light_dir), 0.0);
    float specular_shading = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    return ColorComponents(
        material.ambient * light_color,
        material.diffuse * diffuse_shading * light_color,
        material.specular * specular_shading * light_color
    );
}
