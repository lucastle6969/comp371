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
uniform float opacity;
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
const int COLOR_TREE = 6;

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
vec4 claculateColor(vec3 colorValue);

vec3 colorValue;
vec3 difValue;
vec3 specValue;
float shiValue;
float fog;
float fog_end = 1;
float fog_start = 0;

uniform vec3 fog_color;
uniform float daytime_value;
uniform float nighttime_value;

void main()
{
    switch (color_type) {

        case COLOR_COORDINATE_AXES:
            color = vec4(ceil(pos), 1.0f);
            break;
        case COLOR_WHITE:
            color = WHITE;
            break;
        case COLOR_HEIGHT:
             color = claculateColor(colorValue = vec3(pos.y));
            break;
        case COLOR_TILE:
            color = claculateColor( colorValue = vec3(0.5f, (entity_position_z * 32 - 1) % 256 / 256.0f, entity_position_x * 32 % 256 / 256.0f));
            break;
        case COLOR_TEXTURE:
            vec4 texel = texture(tex_image, tex_coord);
            if(texel.a < 0.5)
                discard;
            color = texel;

            break;
        case COLOR_LIGHTING: {
            // inspired by tutorial at: https://learnopengl.com/#!Lighting/Basic-Lighting
            color = claculateColor (material.ambient);
            break;
        }
        case COLOR_TREE:
        color = claculateColor(
            colorValue = vec3(
                1.0f - abs(pos.x / 10.0f) - abs(pos.y /1000.0f),
                1.0f,
                1.0f - abs(pos.z/ 10.0f) - abs(pos.y /1000.0f)));
            break;
        default:
            color = WHITE;
            break;
    }

   }

vec4 claculateColor(vec3 value){
    colorValue = value;
   // inspired by tutorial at: https://learnopengl.com/#!Lighting/Basic-Lighting
    vec3 normal = normalize(worldNormal);
    vec3 view_dir = normalize(worldViewPos - worldPos);
    // we're passing in the direction of the sunlight but we want the vector
    // pointing TOWARD the sun!
    vec3 sun_dir = -1.0f * normalize(sunLight.direction);
    vec3 pointlight_dir = normalize(pointLight.position - worldPos);
    float point_distance = length(pointLight.position - worldPos);
    // fog thats right we have fog
    fog = (fog_end - length(point_distance))/(fog_end-fog_start);
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
       (max(sunlight_components.ambient, 0) * nighttime_value) +
       (max(pointlight_components.ambient * attenuation, 0) * daytime_value);
    vec3 diffuseValue =
      (max(sunlight_components.diffuse, 0) * nighttime_value) +
       (max(pointlight_components.diffuse * attenuation, 0) * daytime_value);
    vec3 specularValue =
       (max(sunlight_components.specular, 0)*  nighttime_value) +
       (max(pointlight_components.specular * attenuation, 0) * daytime_value);

    if (use_texture) {
       // multiply components against texture value but only if we've
       // got a texture!
       vec3 tex3 = vec3(texture(tex_image, tex_coord));
       ambientValue *= tex3;
       //diffuseValue *= tex3;
       //specularValue *= tex3;
    }

    return vec4(mix(fog_color, ambientValue + diffuseValue + specularValue, fog), opacity);
}

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
        colorValue * light_color,
        material.diffuse * diffuse_shading * light_color,
        material.specular * specular_shading * light_color
    );
}
