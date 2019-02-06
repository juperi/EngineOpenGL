#version 330 core

in vec3 myColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
  fragColor = vec4(myColor.x, myColor.y, myColor.z, 1.0); // Interpolate colors
  fragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), fragColor); //Mix with textures
  // Final result may look different doe to mix with textures
}