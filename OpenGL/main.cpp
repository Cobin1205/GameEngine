#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include "player.h"
#include "Terrain.h"
#include "Pyramid.h"
#include <random>

// GLFW is a framework for creating windows, 
//manage OpenGL contexts, and handle user input 

//GLAD is a library that dynamically loads OpenGL function addresses at runtime

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Callback for resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

//Scene Objects
Player player = Player();

int main() {

	//random stuff
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> colDist(0,1);
	std::uniform_int_distribution<> xzDist(-200, 200);
	std::uniform_int_distribution<> yDist(0, 100);

	//Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create glfw window
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Project", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Make the context of the window the main context on the current thread
	glfwMakeContextCurrent(window);

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Tell OpenGL the window size
	//First two parameters tell OpenGL that (0,0) is at the bottom left of the screen
	//Last two parameters tell OpenGL the width and height of the window
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shader("./vertexShader.vert", "./fragmentShader.frag");

	glEnable(GL_DEPTH_TEST);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(70.0f), 800.0f / 600.0f, 0.1f, 10000.0f);

	//Disable cursor and set cursor functionality
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	//Initialize objects
	Terrain terrain = Terrain();
	terrain.setupMesh();

	Pyramid bigolsahur = Pyramid();
	unsigned int sahurTex = bigolsahur.useTexture("TheTripleT.jpg");
	bigolsahur.setScale(glm::vec3(1000.0f));
	bigolsahur.setPosition(glm::vec3(0.0f, -500.0f, 0.0f));

	//Make a ton of Pyramids
	const int numPyramids = 1000;
	Pyramid pyramids[numPyramids];
	for (int i = 0; i < numPyramids; i++) {
		pyramids[i].useColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		pyramids[i].setPosition(glm::vec3(xzDist(gen), yDist(gen), xzDist(gen)));
	}
	

	//Main program loop
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Process user input such as mouse and key input
		processInput(window, deltaTime);

		//Clear the screen
		glClearColor(0.5f, 0.78f, 0.89f, 1.0f);

		//Use the shader program
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		shader.setInt("ourTexture", 0);

		player.setDirection();

		//Projection Matrices
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(player.cameraPos, player.cameraPos + player.cameraFront, player.cameraUp);
		int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Terrain Draw
		terrain.draw(shader);

		//Draw pyramids
		for (int i = 0; i < numPyramids; i++) {
			pyramids[i].rotate(glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * 500.0f);
			pyramids[i].draw(shader);
		}

		bigolsahur.rotate(glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * 10.0f);
		bigolsahur.draw(shader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Terminate GLFW
	glfwTerminate();

	return 0;
}

//Process user input such as player movement
void processInput(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	player.move(window, deltaTime);
}

//Do this each time the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	player.look(xpos, ypos);
}