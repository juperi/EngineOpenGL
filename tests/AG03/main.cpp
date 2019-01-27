#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdint>
#include "shader.h"

void onChangeframeBufferSize(GLFWwindow* window, const int32_t width, 
	const int32_t height) {
	glViewport(0, 0, width, height);
}

void handlerInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

uint32_t createVertexData(uint32_t *VBO, uint32_t *EBO) {	//Create VAO that compiles everything
	//One triangle
	float vertices[] = { 0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	//top right. Red
						0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	//bottom right. Green
						-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f };	//bottom left. Blue

	uint32_t indices[] = { 0,2,1 };	//Order

	//Generate vertex and elements
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	//Bind Buffers and Upload vertex and elements
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//18 elements

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);	//3 elements

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);	//6 stride
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		(void*) (3 * sizeof(float)));	//Start in 3
	glEnableVertexAttribArray(1);

	//Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //Unbind EBO AFTER unbinding VAO

	return VAO;
}

void render(uint32_t VAO, const Shader& shader) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	shader.use();
	shader.set("uColor", 0.6f, 0.3f, 0.2f);	//To directly set the color with uniform global (Data CPU -> GPU)
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
	window = glfwCreateWindow(800, 600, "AG03", NULL, NULL);
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

	Shader shader("../tests/AG03/vertex.vs", "../tests/AG03/fragment.fs");	//Shaders path
	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);	//Create Vertex Array Object that compiles everything

	glClearColor(0.4f, 0.7f, 0.7f, 1.0f);	//Clear befor entering main loop

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);	
	glCullFace(GL_BACK);	
	
	while(!glfwWindowShouldClose(window)){	//Loop until user closes window
		handlerInput(window);	//Handle Input
		render(VAO, shader);	//Paint
		glfwSwapBuffers(window);	//Swap front and back buffers
		glfwPollEvents();	//Poll for and process events
	}

	//Clean everything
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();	//Close
	return 0;	//Ends OK
}