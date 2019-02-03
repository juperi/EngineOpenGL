
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdint>
#include "shader.h"
#include "camera.h"

#include <stb_image.h>

uint32_t screen_width = 800;
uint32_t screen_height = 600;

float lastFrame = 0.0f;

bool firstMouse = true;
float lastX = (float)screen_width / 2.0f;
float lastY = (float)screen_height / 2.0f;

Camera camera(glm::vec3(-1.0f, 1.5f, 3.0f));

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void onChangeframeBufferSize(GLFWwindow* window, const int32_t width,
	const int32_t height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, width, height);
}

void handlerInput(GLFWwindow* window, const float dt) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.handleKeyboard(Camera::Movement::Forward, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.handleKeyboard(Camera::Movement::Backward, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.handleKeyboard(Camera::Movement::Left, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.handleKeyboard(Camera::Movement::Right, dt);
	}
}

void onScroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.handleMouseScroll(yoffset);
}

void onMouse(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMovement(xoffset, yoffset);
}

uint32_t createQuadVertexData() {
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);

	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	glm::vec3 normal(0.0f, 0.0f, 1.0f);

	glm::vec3 tangent1, tangent2, bitangent1, bitangent2;

	/* 1st Triangle */
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);

	/* 2nd Triangle */
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);

	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);

	float vertices[] = {
		pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, normal.x, normal.y, normal.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, normal.x, normal.y, normal.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
	};

	uint32_t indices[] = {
		0, 1, 2,
		3, 4, 5
	};

	// Generate vertex and elements
	uint32_t VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind Buffers and Upload vertex and elements
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0); // 14 vertex stride
	glEnableVertexAttribArray(0);

	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float))); // Start in 3
	glEnableVertexAttribArray(1);

	// UVs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float))); // Start in 6
	glEnableVertexAttribArray(2);

	// Tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float))); // Start in 8
	glEnableVertexAttribArray(3);

	// Bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float))); // Start in 1
	glEnableVertexAttribArray(4);

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO AFTER unbinding VAO

	return VAO;
}

uint32_t createTexture(const char* path) {
	// Create texture
	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Wraapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload texture and generate Mipmap
	int width, height, nChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed To Load Texture " << path << std::endl;
	}

	return texture;
}

void render(uint32_t VAO, const Shader& shader_bump, const Shader& shader_spec, const uint32_t tex_dif, const uint32_t tex_spec, const uint32_t tex_norm) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// View matix
	glm::mat4 view = camera.getViewMatrix();
	
	// Proj matrix
	glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)screen_width / screen_height, 0.1f, 10.0f);

	// Move light
	float l_pos[] = { std::sin((float)glfwGetTime() / 2.0f), 0.0f, std::abs(cos((float)glfwGetTime() / 2.0f)) };
	lightPos = glm::vec3(l_pos[0], l_pos[1], l_pos[2]);
	
	{
		// Set shader bump
		shader_bump.use();
		shader_bump.set("view", view);
		shader_bump.set("proj", proj);
		glm::mat4 model = glm::mat4(1.0f);	// Identity
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		shader_bump.set("model", model);
		glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
		shader_bump.set("normalMat", normalMat);

		shader_bump.set("viewPos", camera.getPosition());

		shader_bump.set("light.position", lightPos);
		shader_bump.set("light.ambient", 0.1f, 0.1f, 0.1f);
		shader_bump.set("light.diffuse", 0.5f, 0.5f, 0.5f);
		shader_bump.set("light.specular", 1.0f, 1.0f, 1.0f);

		shader_bump.set("material.diffuse", 0);
		shader_bump.set("material.specular", 1);
		shader_bump.set("material.normal", 2);
		shader_bump.set("material.shininess", 25.6f);

		// Active and Upload texture bump
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_dif);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_spec);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex_norm);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// Quad
	}

	{
		// Set shader spec
		shader_spec.use();
		shader_spec.set("view", view);
		shader_spec.set("proj", proj);
		glm::mat4 model = glm::mat4(1.0f);	// Identity
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
		shader_spec.set("model", model);
		glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
		shader_spec.set("normalMat", normalMat);

		shader_spec.set("viewPos", camera.getPosition());

		shader_spec.set("light.position", lightPos);
		shader_spec.set("light.ambient", 0.1f, 0.1f, 0.1f);
		shader_spec.set("light.diffuse", 0.5f, 0.5f, 0.5f);
		shader_spec.set("light.specular", 1.0f, 1.0f, 1.0f);

		shader_spec.set("material.diffuse", 0);
		shader_spec.set("material.specular", 1);
		shader_spec.set("material.shininess", 25.6f);

		// Active and Upload texture spec
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_dif);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_spec);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// Quad
	}

	glBindVertexArray(0);
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	//Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Core Profile

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "AG11", NULL, NULL);
	if (!window) {
		std::cout << "Failed To Create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	// Make the window's context current

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {	// Init GLAD
		std::cout << "Failed To Initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onChangeframeBufferSize);	// ViewPort Callback
	glfwSetCursorPosCallback(window, onMouse);
	glfwSetScrollCallback(window, onScroll);

	// Shaders path
	Shader shader_bump("../tests/AG11/bump.vs", "../tests/AG11/bump.fs");
	Shader shader_spec("../tests/AG11/cube.vs", "../tests/AG11/cube.fs");
	
	uint32_t VAO = createQuadVertexData();	

	// Texture path
	uint32_t tex_dif = createTexture("../tests/AG11/albedo.png");
	uint32_t tex_spec = createTexture("../tests/AG11/specular.png");
	uint32_t tex_norm = createTexture("../tests/AG11/normal.png");

	stbi_set_flip_vertically_on_load(true); // Avoid to load the image reversed

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear before entering main loop

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(window)) {	// Loop until user closes window
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handlerInput(window, deltaTime); // Handle Input

		render(VAO, shader_bump, shader_spec, tex_dif, tex_spec, tex_norm); // Paint

		glfwSwapBuffers(window); // Swap front and back buffers

		glfwPollEvents(); // Poll for and process events
	}

	glDeleteVertexArrays(1, &VAO); 	// Clean

	glfwTerminate();	// Close
	return 0;	// Ends OK
}