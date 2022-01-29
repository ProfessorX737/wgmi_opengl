#version 330 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vColor;
in vec3 vPosition;
in vec3 vView;
in vec3 vTexDir;
noperspective in vec2 vScreenCoord;

layout (location = 0) out vec4 fFragColor;

uniform sampler2D uDiffuseMap;
uniform float uDiffuseMapFactor;

uniform sampler2D uSpecularMap;
uniform float uSpecularMapFactor;

uniform samplerCube uCubeMap;
uniform float uCubeMapFactor;

uniform sampler2D uNormalMap;
uniform float uNormalMapFactor;

uniform sampler2D uAmbientMap;
uniform float uAmbientMapFactor;

uniform sampler2D uRoughnessMap;
uniform float uRoughnessMapFactor;

uniform sampler2D uReflectionMap;
uniform float uReflectionMapFactor;

struct Material {
    vec3 ambient;
    vec4 diffuse;
    vec3 specular;
    float phongExp;
};

struct DirLight {
    vec3 direction;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

uniform Material uMat;
uniform DirLight uSun;
uniform SpotLight uSpot;

uniform mat4 uModel;
uniform vec3 uCameraPos;
uniform bool uIsWaterSurface;

vec3 fNormal;
vec3 fView;
float fShininess;

vec3 sRGB_to_linear(vec3 col) {
    return pow(col, vec3(2.2));
}

vec3 linear_to_sRGB(vec3 col) {
    return pow(col, vec3(1/2.2));
}

vec3 calc_dir_light(DirLight light, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 ambient = light.ambient * mat_diffuse * mat_ambient;
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light.direction, fNormal));
    vec3 reflected = reflect(light.direction, vNormal);
    vec3 specular = light.specular * mat_specular * pow(max(0, dot(reflected, fView)), fShininess);
    return ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 ambient = light.ambient * mat_diffuse * mat_ambient;
    vec3 light_direction = normalize(vPosition - light.position);
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light_direction, fNormal));
    vec3 reflected = reflect(light_direction, fNormal);
    vec3 specular = light.specular * mat_specular * pow(max(0, dot(reflected, fView)), fShininess);
    float distance = distance(light.position, vPosition);
    return (ambient + diffuse + specular);
}

void main() {
    fView = normalize(vView);

    fShininess = mix(uMat.phongExp, 1/(texture(uRoughnessMap, vTexCoord).r + 0.001), uRoughnessMapFactor);

    fNormal = mix(normalize(vNormal), normalize(mat3(uModel) * (texture(uNormalMap, vTexCoord).xyz * 2.0 - 1.0)), uNormalMapFactor);

    vec3 mat_ambient = mix(uMat.ambient, texture(uAmbientMap, vTexCoord).rgb, uAmbientMapFactor);
    mat_ambient = sRGB_to_linear(mat_ambient);

    vec2 diffuseTexCoord = vTexCoord;
    if (uIsWaterSurface) {
        diffuseTexCoord = vScreenCoord;
        //        diffuseTexCoord += fNormal.xz * 0.05;
        diffuseTexCoord = clamp(diffuseTexCoord, 0, 1);
    }

    vec4 diffuse = uMat.diffuse;
    diffuse.xyz += normalize(vColor);
    vec4 mat_diffuse = mix(diffuse, texture(uDiffuseMap, diffuseTexCoord), uDiffuseMapFactor);
    vec3 color = vec3(mix(diffuse, texture(uDiffuseMap, diffuseTexCoord), uDiffuseMapFactor));
    // calculate texture direction for cubemap
    vec3 vTexDir = reflect(-fView, fNormal);
    mat_diffuse.rgb = mix(mat_diffuse, texture(uCubeMap, vTexDir), uCubeMapFactor).rgb;
    vec2 reflectionTexCoord = diffuseTexCoord;
    reflectionTexCoord.y = 1 - diffuseTexCoord.y;
    mat_diffuse.rgb = mix(mat_diffuse, texture(uReflectionMap, reflectionTexCoord), uReflectionMapFactor).rgb;
    mat_diffuse.rgb = sRGB_to_linear(mat_diffuse.rgb);

    vec3 mat_specular = mix(uMat.specular, texture(uSpecularMap, vTexCoord).rgb, uSpecularMapFactor);
    mat_specular = sRGB_to_linear(mat_specular);

    DirLight sun = uSun;
    sun.ambient = sRGB_to_linear(sun.ambient);
    sun.diffuse = sRGB_to_linear(sun.diffuse);
    sun.specular = sRGB_to_linear(sun.specular);

    SpotLight spot = uSpot;
    spot.ambient = sRGB_to_linear(spot.ambient);
    spot.diffuse = sRGB_to_linear(spot.diffuse);
    spot.specular = sRGB_to_linear(spot.specular);

    vec3 shade = calc_dir_light(sun, mat_ambient, mat_diffuse.rgb, mat_specular) + calc_spot_light(spot, mat_ambient, mat_diffuse.rgb, mat_specular) * 0.01;
    shade = linear_to_sRGB(shade);
    fFragColor = vec4(shade * 0.001 + color, mat_diffuse.a);
}
