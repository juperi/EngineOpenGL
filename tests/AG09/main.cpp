#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdint>
#include "shader.h"
#include "camera.h"
#include "model.h"

#include <stb_image.h>

uint32_t screen_width = 800;
uint32_t screen_height = 600;

float lastFrame = 0.0f;

bool firstMouse = true;
float lastX = (float)screen_width / 2.0f;
float lastY = (float)screen_height / 2.0f;

Camera camera(glm::vec3(-1.0f, 1.5f, 3.0f));

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


void render(const Shader& shader, const Model& object) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// View matix
	glm::mat4 view = camera.getViewMatrix();
	// Proj matrix
	glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), (float)screen_width / screen_height, 0.1f, 60.0f);

	// Upload matrix to shader
	shader.use();
	shader.set("view", view);
	shader.set("proj", proj);
	
	glm::mat4 model(1.0f); // Identity
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	shader.set("model", model);

	object.Draw(shader);

	glBindVertexArray(0);
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	// Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Core Profile

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "AG09", NULL, NULL);
	if (!window) {
		std::cout << "Failed To Create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	// Make the window's context current

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 	// Init GLAD
		std::cout << "Failed To Initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onChangeframeBufferSize); // ViewPort Callback
	glfwSetCursorPosCallback(window, onMouse);
	glfwSetScrollCallback(window, onScroll);

	// Shaders path
	Shader shader("../tests/AG09/shader.vs", "../tests/AG09/shader.fs");
	
	// Load model
	Model object("../assets/Freighter/Freigther_BI_Export.obj");

	// Clear befor entering main loop
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Loop until user closes window
	while (!glfwWindowShouldClose(window)) {	
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handlerInput(window, deltaTime); // Handle keyboard
		
		render(shader, object); // Paint
		
		glfwSwapBuffers(window); // Swap front and back buffers
		
		glfwPollEvents(); // Poll for and process events
	}

	glfwTerminate(); // Close
	return 0; //Ends OK
}