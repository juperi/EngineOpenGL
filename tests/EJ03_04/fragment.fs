#version 330 core

in vec3 myColor;
in vec4 myPos;

out vec4 fragColor;

void main() {
  fragColor = vec4(myPos.x, myPos.y, myPos.z, 1.0);
}