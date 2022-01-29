#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 vTexCoord;
out vec3 vColor;
out vec3 vNormal;
out vec3 vPosition;
out vec3 vView;
noperspective out vec2 vScreenCoord;

uniform mat4 uViewProj;
uniform mat4 uModel;
uniform vec3 uCameraPos;
//uniform vec4 uClipPlane;
uniform bool uIsWaterSurface;
uniform bool uIsWater;
uniform float uNow;

uniform float uRainbow;
uniform float uColorOffset;
uniform mat4 uColorRotation;

uniform sampler2D uHeightMap;

out float gl_ClipDistance[1];

float calc_water_height(vec3 pos) {
    return pos.y + 0.1 * (0.8 * sin(pos.x + uNow) + 0.4 * cos(pos.z + uNow));
}

void main() {
    vTexCoord = aTexCoord;
    vColor = mix(aColor,normalize(uColorOffset + vec3(uColorRotation * aPos)), uRainbow);
    vNormal = normalize(mat3(uModel) * aNormal);
    vec4 pos = uModel * aPos;
    // if water then use sin/cos to alter the height
    if (uIsWater) {
        pos.y = calc_water_height(pos.xyz);
        if (uIsWaterSurface) {
            // calculate normal for the distorted mesh
            float offset = 0.1;
            vec3 top = vec3(pos.x, pos.y, pos.z - offset);
            top.y = calc_water_height(top);
            vec3 left = vec3(pos.x - offset, pos.y, pos.z + offset);
            left.y = calc_water_height(left);
            vec3 right = vec3(pos.x + offset, pos.y, pos.z + offset);
            right.y = calc_water_height(right);
            vec3 a = left - top;
            vec3 b = right - top;
            vNormal = normalize(cross(a, b));
        }
    }
    pos.y += texture(uHeightMap, vTexCoord).r;
    vPosition = pos.xyz;
    vView = normalize(uCameraPos - vPosition);
    gl_Position = uViewProj * pos;

    vScreenCoord = (gl_Position.xy / gl_Position.w) * 0.5 + vec2(0.5);

    // set gl_ClipDistance[0] to be the distance between vPosition and the clipping plane
//    gl_ClipDistance[0] = dot(pos, uClipPlane);
    gl_ClipDistance[0] = -pos.z;
}
