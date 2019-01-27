#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdint>
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

void onChangeframeBufferSize(GLFWwindow* window, const int32_t width, 
	const int32_t height) {
	glViewport(0, 0, width, height);
}

void handlerInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

uint32_t createVertexData(uint32_t *VBO, uint32_t *EBO) {	// Create VAO that compiles everything
	//Cube
	float vertices[] = {
		// Position				// UVs
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, //Front
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, //Right
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, //Back
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, //Left
		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f, //Bottom
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, //Top
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f
	};
	uint32_t indices[] = {
		0, 1, 2,		0, 2, 3,	//Front
		4, 5, 6,		4, 6, 7,	//Right
		8, 9, 10,		8, 10, 11,	//Back
		12, 13, 14,		12, 14, 15, //Left
		16, 17, 18,		16, 18, 19, //Bottom
		20, 21, 22,		20, 22, 23	//Top
	};

	//Generate vertex and elements
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	//Bind Buffers and Upload vertex and elements
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);	

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);	
	glEnableVertexAttribArray(0);

	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
		(void*) (3 * sizeof(float)));	//Start in 3
	glEnableVertexAttribArray(1);

	//Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //Unbind EBO AFTER unbinding VAO

	return VAO;
}

uint32_t createTexture(const char* path) {
	//Create texture
	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Wraapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//Upload texture and generate Mipmap
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

void render(uint32_t VAO, const Shader& shader, const uint32_t tex1, const uint32_t tex2) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	shader.use();
	glBindVertexArray(VAO);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);

	shader.set("texture1", 0);
	shader.set("texture2", 1);

	//TRS matrix
	glm::mat4 view = glm::mat4(1.0f);	//Identity
	view = glm::translate(view,glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 proj = glm::mat4(1.0f);	//Identity
	proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 60.0f);

	shader.set("view", view);
	shader.set("proj", proj);

	for (uint8_t i = 0; i < 10; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 10.0f + (20.0f * i);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
		shader.set("model", model);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	//6*2*3
	}
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	//Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Core Profile

	GLFWwindow* window;	//Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "AG05_01", NULL, NULL);
	if (!window) {
		std::cout << "Failed To Create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	//Make the window's context current
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {	//Init GLAD
		std::cout << "Failed To Initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onChangeframeBufferSize);	//ViewPort Callback

	Shader shader("../tests/AG05_01/vertex.vs", "../tests/AG05_01/fragment.fs");	//Shaders path
	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);	//Create Vertex Array Object that compiles everything
	
	//Avoid to load the image reversed
	stbi_set_flip_vertically_on_load(true);

	//Create texture from images
	uint32_t tex1 = createTexture("../tests/AG05_01/image.jpg");
	uint32_t tex2 = createTexture("../tests/AG05_01/image2.jpg");

	glClearColor(0.4f, 0.7f, 0.7f, 1.0f);	//Clear befor entering main loop

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);	
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);	//Near cam
	
	while(!glfwWindowShouldClose(window)){	//Loop until user closes window
		handlerInput(window);	//Handle Input
		render(VAO, shader, tex1, tex2);	//Paint
		glfwSwapBuffers(window);	//Swap front and back buffers
		glfwPollEvents();	//Poll for and process events
	}

	//Clean everything
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);

	glfwTerminate();	//Close
	return 0;	//Ends OK
}