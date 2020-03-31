#version 420 core

out vec4 FragColor;

uniform float lightIntensity;

void main(){
    FragColor = vec4(lightIntensity);
}
