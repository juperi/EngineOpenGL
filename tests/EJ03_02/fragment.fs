#version 330 core

in vec3 myColor;
out vec4 fragColor;

uniform vec3 uColor;

void main() {
  fragColor = vec4(uColor, 1.0);
}