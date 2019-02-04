#version 330 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main(){
	vec2 offset[9] = vec2[] (
		vec2(-offset, offset),
		vec2(0.0, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0),
		vec2(0.0, 0.0),
		vec2(offset, 0.0),
		vec2(-offset, -offset),
		vec2(0.0, -offset),
		vec2(offset, -offset)
	);

	float kernel[9] = float[] (
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);

	float blur[9] = float[] (
		1.0/16, 2.0/16, 1.0/16,
		2.0/16, 4.0/16, 2.0/16,
		1.0/16, 2.0/16, 1.0/16
	);

	float borderDet[9] = float[] (
		1,  1, 1,
		1, -8, 1,
		1,  1, 1
	);

	vec3 sampleTex[9];
	for (int i= 0; i < 9; ++i){
		sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offset[i]));
	}
	vec3 color = vec3(0.0);
	for (int i= 0; i < 9; ++i){
		//color += sampleTex[i] * kernel[i];
		//color += sampleTex[i] * blur[i];
		color += sampleTex[i] * borderDet[i];
	}
	//vec4 color = texture(screenTexture, texCoords);
	//float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	//fragColor = vec4(average, average, average, 1.0);
	fragColor = vec4(color, 1.0);
}