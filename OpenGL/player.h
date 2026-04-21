#include <glm.hpp>
#include "RigidBody.hpp"

class Player : RigidBody {
public:
	//Camera variables
	glm::vec3 cameraPos = glm::vec3(0.0f, 100.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 400;
	float lastY = 300;

	//Physics stuff
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
	bool onGround;

	//Set where the camera should look each frame
	void setDirection() {
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

	//move the player with WASD
	void move(GLFWwindow* window, float deltaTime, float (perlin(float, float)) ) {
		glm::vec3 moveDir = glm::vec3(0.0f, 0.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			moveDir += cameraFront * glm::vec3(1.0f, 0.0f, 1.0f);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			moveDir -= cameraFront * glm::vec3(1.0f, 0.0f, 1.0f);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			moveDir -= glm::normalize(glm::cross(cameraFront, cameraUp)) * glm::vec3(1.0f, 0.0f, 1.0f);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			moveDir += glm::normalize(glm::cross(cameraFront, cameraUp)) * glm::vec3(1.0f, 0.0f, 1.0f);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround)
			velocity.y = 2.0f;

		if (glm::length(moveDir) > 0) {
			moveDir = glm::normalize(moveDir) * 0.2f;
		}

		//update velocity
		velocity += moveDir * 100.0f*deltaTime;
		velocity.y -= 5.0f * deltaTime;

		//Friction
		velocity.x *= 0.95f;
		velocity.z *= 0.95f;

		cameraPos += velocity;

		//Correct collision with floor
		float xNorm, zNorm;
		xNorm = ((float)cameraPos.x/100.0f + 100.0f) / 200.0f;
		zNorm = ((float)cameraPos.z/100.0f + 100.0f) / 200.0f;
		float groundHeight = perlin(xNorm * 10, zNorm * 10) * 1000;
		if (cameraPos.y < groundHeight + 50.0f) {
			cameraPos.y = groundHeight + 50.0f;
			velocity.y = 0.0f;
			onGround = true;
		}
		else {
			onGround = false;
		}
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