#include <glm.hpp>
#include "RigidBody.hpp"

class Player : RigidBody {
public:
	//Camera variables
	glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 direction;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 400;
	float lastY = 300;

	float moveSpeed = 0.05f;

	//Set where the camera should look each frame
	void setDirection() {
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

	//move the player with WASD
	void move(GLFWwindow* window, float deltaTime) {
		glm::vec3 moveDir = glm::vec3(0.0f, 0.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			moveDir += cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			moveDir -= cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			moveDir -= glm::normalize(glm::cross(cameraFront, cameraUp));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			moveDir += glm::normalize(glm::cross(cameraFront, cameraUp));

		if (glm::length(moveDir) > 0) {
			moveDir = glm::normalize(moveDir) * 0.2f;
		}

		cameraPos += moveDir * moveSpeed;
	}

	//Make player look around on mouse movement
	void look(float xpos, float ypos) {
		float xOffset = xpos - lastX;
		float yOffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;

		const float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= -sensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
};