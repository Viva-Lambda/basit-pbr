#version 330 core
// fragment shader for pbr

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;


// texture related
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

// light pos
uniform vec3 lightPos;

// functions
vec3 getSurfaceNormal();
vec3 getLightDir();
vec3 getDiffuseColor(vec3 ldir, vec3 normal, vec3 color);


void main() {
    vec3 albedo = texture(diffuseMap, TexCoord).rgb;
    vec3 surfaceNormal = getSurfaceNormal();
    vec3 lightDirection = getLightDir();
    vec3 diffuseColor = getDiffuseColor(lightDirection, surfaceNormal, albedo);

    FragColor = vec4(diffuseColor, 1.0);
}

vec3 getLightDir() {
    return normalize(lightPos - FragPos); }
vec3 getSurfaceNormal() {
  vec3 normal = texture(normalMap, TexCoord).rgb;
  return normalize(normal * 2.0 - 1.0);
}
vec3 getDiffuseColor(vec3 ldir, vec3 normal, vec3 color) {
  float costheta = dot(ldir, normal);
  // opaque surfaces
  return max(costheta, 0.0) * color;
}

