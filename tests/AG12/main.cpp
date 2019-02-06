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

float lastFrame = 0.0f;//

bool firstMouse = true;
float lastX = (float)screen_width / 2.0f;
float lastY = (float)screen_height / 2.0f;

Camera camera(glm::vec3(-1.0f, 2.0f, 6.0f));

glm::vec3 lightPos(1.2f, 1.0f, -2.0f);

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

uint32_t createVertexData(const float* vertices, const uint32_t n_verts, 
	const uint32_t* indices, const uint32_t n_indices) {	// Create VAO that compiles everything
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO); // Create VAO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set Vertex Buffers and then configure vertex attributes
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n_verts * sizeof(float) * 8, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // 3 + 2 + 3 vertex stride
	glEnableVertexAttribArray(0);

	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Starts in 3
	glEnableVertexAttribArray(1);

	// UVs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Starts in 6
	glEnableVertexAttribArray(2);

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindVertexArray(0); // Unbind VAO
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
	stbi_set_flip_vertically_on_load(true);
	int width, height, nChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed To Load Texture " << path << std::endl;
	}
	stbi_image_free(data);

	return texture;
}

std::pair<uint32_t, uint32_t> createFBO() {
	uint32_t fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Texture attached to Frame Buffer
	uint32_t textureColor;
	glGenFramebuffers(1, &textureColor);
	glBindTexture(GL_TEXTURE_2D, textureColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0);

	// RenderBuffer attached to FBO
	uint32_t rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screen_width, screen_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error FrameBuffer Not Complete" << std::endl;
	}
	
	return std::make_pair(fbo, textureColor);
}

void render(const Shader& lightingShader, const Shader& fboShader, const uint32_t cubeVAO, const uint32_t quadVAO, const uint32_t quadScreenVAO,
	const uint32_t tex1, const uint32_t tex2, const uint32_t fbo, const uint32_t tex_fbo) {
	
	// 1ST PASS _______________________________________________________

	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);

	// Light shader
	lightingShader.use();
	lightingShader.set("objectColor", 1.0f, 0.5f, 0.31f);

	lightingShader.set("viewPos", camera.getPosition());

	lightingShader.set("light.position", lightPos);
	lightingShader.set("light.ambient", 0.2f, 0.2f, 0.2f);
	lightingShader.set("light.diffuse", 0.2f, 1.0f, 0.2f);
	lightingShader.set("light.specular", 1.0f, 1.0f, 1.0f);

	lightingShader.set("light.diffuse", 0.2f, 1.0f, 0.2f);

	lightingShader.set("material.ambient", 1.0f, 0.5f, 0.31f);
	lightingShader.set("material.shininess", 32.2f);
	lightingShader.set("material.diffuse", 0);
	lightingShader.set("material.specular", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);

	// View matix
	glm::mat4 view = camera.getViewMatrix();
	lightingShader.set("view", view);
	
	// Proj matrix
	glm::mat4 proj = glm::mat4(1.0f);	// Identity
	proj = glm::perspective(glm::radians(camera.getFOV()), (float)screen_width / screen_height, 0.1f, 100.0f);
	lightingShader.set("proj", proj);
	
	/* QUAD */

	// Model matrix
	glm::mat4 model = glm::mat4(1.0f);	// Identity
	model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
	lightingShader.set("model", model);

	// Normal matrix
	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	/* 3 CUBE */

	// Model matrix 1
	model = glm::mat4(1.0f);	// Identity
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, 1.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.set("model", model);

	// Normal matrix 1
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Model matrix 2
	model = glm::mat4(1.0f);	// Identity
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, 0.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.set("model", model);

	// Normal matrix 2
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Model matrix 3
	model = glm::mat4(1.0f);	// Identity
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, -1.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.set("model", model);

	// Normal matrix 3
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // No need to unbind it every time

	// 2ND PASS _______________________________________________________

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // 2D quad

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	fboShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_fbo);
	fboShader.set("screenTexture", 0);

	glBindVertexArray(quadScreenVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0); // No need to unbind it every time
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	//Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Core Profile

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "AG12", NULL, NULL);
	if (!window) {
		std::cout << "Failed To Create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	// Make the window's context current
	glfwSetFramebufferSizeCallback(window, onChangeframeBufferSize); // ViewPort Callback
	glfwSetCursorPosCallback(window, onMouse);
	glfwSetScrollCallback(window, onScroll);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {	// Init GLAD
		std::cout << "Failed To Initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	Shader lightingShader("../tests/AG12/cube.vs", "../tests/AG12/cube.fs");
	Shader fboShader("../tests/AG12/fbo.vs", "../tests/AG12/fbo.fs");

	float cube_vertices[] = {
		// Position				// Normals				// UVs		
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,	// Front
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,

		0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,	// Right
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f, 		1.0f, 0.0f,	// Back
		-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f, 		1.0f, 0.0f,	// Left
		-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f, 		0.0f, 1.0f,	// Bottom
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,	// Top
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	};

	uint32_t cube_indices[] = {
		0, 1, 2,		0, 2, 3,	// Front
		4, 5, 6,		4, 6, 7,	// Right
		8, 9, 10,		8, 10, 11,	// Back
		12, 13, 14,		12, 14, 15, // Left
		16, 17, 18,		16, 18, 19, // Bottom
		20, 21, 22,		20, 22, 23	// Top
	};

	/* Floor */
	float quad_vertices[] = {
		// Position				// Colors				// Texture
		-0.5f, 0.0f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,	// top
		 0.5f, 0.0f,  0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,	
		 0.5f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,	
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f	
	};

	uint32_t quad_indices[] = {
		0,1,2,
		0,2,3
	};

	/* Screen */
	float quad_screen_vertices[] = {
		// Position				// Colors				// Texture
		-1.0f,  1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, // top
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f
	};

	uint32_t quad_screen_indices[] = {
		0,1,2,
		0,2,3
	};

	uint32_t cubeVAO = createVertexData(cube_vertices, 24, cube_indices, 36);
	uint32_t quadVAO = createVertexData(quad_vertices, 4, quad_indices, 6);
	uint32_t quadScreenVAO = createVertexData(quad_screen_vertices, 4, quad_screen_indices, 6);

	uint32_t tex1 = createTexture("../tests/AG12/albedo.png");
	uint32_t tex2 = createTexture("../tests/AG12/specular.png");

	auto fbo_res = createFBO();

	while (!glfwWindowShouldClose(window)) { // Loop until user closes window
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handlerInput(window, deltaTime);
		
		render(lightingShader, fboShader, cubeVAO, quadVAO, quadScreenVAO, tex1, tex2, fbo_res.first, fbo_res.second); // Paint
		
		glfwSwapBuffers(window); // Swap front and back buffers
		
		glfwPollEvents(); // Poll for and process events
	}

	glDeleteVertexArrays(1, &cubeVAO); // Deallocate resuorces
	glDeleteVertexArrays(1, &quadVAO); // Deallocate resuorces
	glDeleteVertexArrays(1, &quadScreenVAO); // Deallocate resuorces

	glfwTerminate(); // Close
	return 0; // Ends OK
}