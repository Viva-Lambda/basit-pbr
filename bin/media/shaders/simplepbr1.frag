#version 330 core
// fragment shader for pbr

out vec4 FragColor;

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;


// texture related
uniform sampler2D albedoMap;


void main() {
    vec3 albedo = texture(albedoMap, TexCoord).rgb;

    FragColor = vec4(albedo, 1.0);
}
