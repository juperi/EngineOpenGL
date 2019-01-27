#ifndef __CAMERA_H__
#define __CAMERA_H__ 1

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Default Camera Values
const float k_Yaw = -90.0f;
const float k_Pitch = 0.0f;
const float k_Speed = 2.5f;
const float k_Sensitivity = 0.1f;
const float k_FOV = 45.0f;
class Camera {
public:
	enum class Movement { //Possible options for camera movement
		Forward = 0,
		Backward = 1,
		Left = 2,
		Right = 3,
	};

	//Empty Constructor
	Camera(); 
	//Destructor
	~Camera(); 

	//Constructor with Vectors
	Camera(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		const float yaw = k_Yaw, const float pitch = k_Pitch);
	//Constructor with Scalars
	Camera(const float posX, const float posY, const float posZ, const float upX, const float upY, const float upZ,
		const float yaw, const float pitch);

	//Returns current view matrix
	glm::mat4 getViewMatrix() const; 
	//Returns the FOV
	float getFOV() const; 
	//Returns Position
	glm::vec3 getPosition() const;

	//Process input from keyboard
	void handleKeyboard(const Movement direction, const float dt);
	//Process mouse movement
	void handleMouseMovement(const float xoffset, const float yoffset,
		const bool constrainPitch = true);
	//Process mouse scroll
	void handleMouseScroll(const float yoffset);

private:
	//Calculates front vector
	void updateCameraVectors();

	//Calculates the lookAt View Matrix
	glm::mat4 lookAt() const;

	// Camera Attributes
	glm::vec3 position_, front_, up_, right_, worldUp_; 
	// Euler Angles
	float yaw_, pitch_;
	// Camera options
	float fov_; 
	// Camera can fly
	bool flying_ = true;
};

#endif