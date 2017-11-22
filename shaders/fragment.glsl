#version 330 core

in vec3 pos;
in vec3 worldPos;
in vec3 worldNormal;
in vec2 tex_coord;

struct SunLight {
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

ColorComponents calculateColor(const vec3 light_dir, const vec3 normal, const vec3 view_dir);

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
                normal,
                view_dir
            );
            ColorComponents pointlight_components = calculateColor(
                pointlight_dir,
                normal,
                view_dir
            );

            vec3 ambientValue =
                sunlight_components.ambient +
                pointlight_components.ambient * attenuation;
            vec3 diffuseValue =
                sunlight_components.diffuse +
                pointlight_components.diffuse * attenuation;
            vec3 specularValue =
                sunlight_components.specular +
                pointlight_components.specular * attenuation;

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

// compute the basic color without considering attenuation or texture multiplication
ColorComponents calculateColor(const vec3 light_dir, const vec3 normal, const vec3 view_dir)
{
    vec3 reflect_dir = reflect(-light_dir, normal);

    // compute diffuse and specular shading
    float diffuse_shading = max(dot(normal, light_dir), 0.0);
    float specular_shading = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    return ColorComponents(
        material.ambient,
        material.diffuse * diffuse_shading,
        material.specular * specular_shading
    );
}
