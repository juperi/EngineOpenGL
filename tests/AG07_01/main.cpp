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

uint32_t createVertexData(uint32_t *VBO, uint32_t *EBO) {	// Create VAO that compiles everything
	//Cube
	float vertices[] = {
		// Position				// UVs			// Normals
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f, //Front
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,

		0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,//Right
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f, 0.0f, -1.0f,//Back
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,//Left
		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,//Bottom
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,//Top
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	// 3 + 2 + 3 vertex stride
	glEnableVertexAttribArray(0);

	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));	//Start in 3
	glEnableVertexAttribArray(1);

	//normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(5 * sizeof(float)));	//Start in 5
	glEnableVertexAttribArray(2);

	//Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //Unbind EBO AFTER unbinding VAO

	return VAO;
}

void render(uint32_t VAO, const Shader& shader_light, const Shader& shader_cube) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//View matix
	glm::mat4 view = camera.getViewMatrix();
	//Proj matrix
	glm::mat4 proj = glm::mat4(1.0f);	//Identity
	proj = glm::perspective(glm::radians(camera.getFOV()), (float)screen_width / screen_height, 0.1f, 10.0f);
	//Model matrix
	glm::mat4 model = glm::mat4(1.0f);	//Identity
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	//Light shader
	shader_light.use();
	shader_light.set("view", view);
	shader_light.set("proj", proj);
	shader_light.set("model", model);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	//6*2*3	

	//Cube shader
	shader_cube.use();
	shader_cube.set("view", view);
	shader_cube.set("proj", proj);
	shader_cube.set("model", glm::mat4(1.0f));

	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader_cube.set("normalMat", normalMat);

	shader_cube.set("viewPos", camera.getPosition());

	shader_cube.set("light.position", lightPos);
	shader_cube.set("light.ambient", 0.2f, 0.15f, 0.1f);
	shader_cube.set("light.diffuse", 0.5f, 0.5f, 0.5f);
	shader_cube.set("light.specular", 1.0f, 1.0f, 1.0f);

	shader_cube.set("material.ambient", 0.2125f, 0.1275f, 0.054f);	//bronze
	shader_cube.set("material.diffuse", 0.714f, 0.4284f, 0.18144f);
	shader_cube.set("material.specular", 0.393548f, 0.271906f, 0.166721f);
	shader_cube.set("material.shininess", 32.0f);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	//6*2*3	

	glBindVertexArray(0);
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	//Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Core Profile

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "AG07_01", NULL, NULL);
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
	glfwSetCursorPosCallback(window, onMouse);
	glfwSetScrollCallback(window, onScroll);

	//Shaders path
	Shader shader_light("../tests/AG07_01/light.vs", "../tests/AG07_01/light.fs");
	Shader shader_cube("../tests/AG07_01/cube.vs", "../tests/AG07_01/cube.fs");
	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);	//Create Vertex Array Object that compiles everything

	//Avoid to load the image reversed
	stbi_set_flip_vertically_on_load(true);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	//Clear befor entering main loop

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(window)) {	//Loop until user closes window
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handlerInput(window, deltaTime);	//Handle Input
		render(VAO, shader_light, shader_cube);	//Paint
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