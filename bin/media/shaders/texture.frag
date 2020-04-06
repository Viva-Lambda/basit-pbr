#version 330 core

out vec4 FragColor;

in vec3 myColor;
in vec2 TexCoord;

uniform sampler2D myTexture;

void main(){
    vec4 tcolor = texture(myTexture, TexCoord);
    //FragColor = vec4(tcolor.x, tcolor.y, tcolor.z, 1.0); // gray
    FragColor = vec4(tcolor.x, tcolor.y, tcolor.z, tcolor.w); // gray
    //FragColor = vec4(tcolor.x, tcolor.z, tcolor.y, 1.0); // gray
    //FragColor = vec4(tcolor.y, tcolor.z, tcolor.x, 1.0); // gray
    //FragColor = vec4(tcolor.y, tcolor.x, tcolor.z, 1.0); // gray
    //FragColor = vec4(tcolor.z, tcolor.y, tcolor.x, 1.0); // gray
    //FragColor = vec4(tcolor.z, tcolor.x, tcolor.y, 1.0); // gray
    //FragColor = vec4(tcolor.x, tcolor.y, tcolor.z, 0.0); // gray
    //FragColor = vec4(tcolor.x, tcolor.z, tcolor.y, 0.0); // gray
    //FragColor = vec4(tcolor.y, tcolor.z, tcolor.x, 0.0); // gray
    //FragColor = vec4(tcolor.y, tcolor.x, tcolor.z, 0.0); // gray
    //FragColor = vec4(tcolor.z, tcolor.y, tcolor.x, 0.0); // gray
    //FragColor = vec4(tcolor.z, tcolor.x, tcolor.y, 0.0); // gray

}
