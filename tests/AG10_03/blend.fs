#version 330 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D tex;

void main() {
	vec4 texColor = texture(tex, texCoords);
	if (texColor.a <=0.1){ // Do not paint when alpha is low
		discard;
	}

	fragColor = texColor;
}