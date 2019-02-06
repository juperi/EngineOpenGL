#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdint>
#include "shader.h"

#include <stb_image.h>

float texOpacity = 0.5f; // Texture opacity variable 0.5 initialization

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
	// Two triangles (QUAD)		
	float vertices[] = {
		// Position				// Colors				// Text Coords
		0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,	// Top right. RED
		0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,	// Bottom right. GREEN
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,	// Bottom left. BLUE
		-0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 1.0f,		0.0f, 1.0f	// Bop left. MAGENTA
	};	

	uint32_t indices[] = {	// Index order 
		0,3,1, 
		1,3,2
	};	

	// Generate vertex and elements
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	// Bind Buffers and Upload vertex and elements
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// 32 elements

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);	// 6 elements

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	// 8 stride
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
		(void*) (3 * sizeof(float)));	// Start in 3
	glEnableVertexAttribArray(1);

	// Text coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float)));	// Start in 6
	glEnableVertexAttribArray(2);

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

	// Avoid to load the image reversed
	stbi_set_flip_vertically_on_load(true);
	
	// Upload texture and generate Mipmap
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

float setTexOpacity(GLFWwindow* window) { // Set texture mix
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { // Add more texture1
		texOpacity += 0.1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { // Add more texture2
		texOpacity -= 0.1;
	}
	return texOpacity;
}

void render(uint32_t VAO, const Shader& shader, const uint32_t tex1, const uint32_t tex2, const float texOpacity) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	shader.use();
	glBindVertexArray(VAO);

	shader.set("texOpacity", texOpacity);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);

	shader.set("texture1", 0);
	shader.set("texture2", 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	// Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core Profile

	GLFWwindow* window;	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "EJ04_05", NULL, NULL);
	if (!window) {
		std::cout << "Failed To Create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	// Make the window's context current
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Init GLAD
		std::cout << "Failed To Initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onChangeframeBufferSize); // ViewPort Callback

	Shader shader("../tests/EJ04_05/vertex.vs", "../tests/EJ04_05/fragment.fs"); // Shaders path
	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO); // Create Vertex Array Object that compiles everything
	
	// Create texture from images
	uint32_t tex1 = createTexture("../tests/EJ04_05/image.jpg");
	uint32_t tex2 = createTexture("../tests/EJ04_05/image2.jpg");

	glClearColor(0.4f, 0.7f, 0.7f, 1.0f);	// Clear befor entering main loop

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);	
	glCullFace(GL_BACK);	
	
	while(!glfwWindowShouldClose(window)){	// Loop until user closes window
		handlerInput(window); // Handle Input

		float texOpacity = setTexOpacity(window); // Call set Texture Opacity method
		
		render(VAO, shader, tex1, tex2, texOpacity); // Paint

		handlerInput(window); // Handle Input
		
		glfwSwapBuffers(window); // Swap front and back buffers
		
		glfwPollEvents(); // Poll for and process events
	}

	// Clean everything
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);

	glfwTerminate(); // Close
	return 0; // Ends OK
}