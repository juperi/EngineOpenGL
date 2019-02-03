#version 330 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int shininess;
uniform float ambientStrenght;
uniform float specularStrenght;

void main(){
	vec3 ambient = ambientStrenght * lightColor;
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	
	float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
	vec3 specular = specularStrenght * spec * lightColor;
	
	vec3 phong = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(phong, 1.0);
}