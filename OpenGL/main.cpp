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

// GLFW is a framework for creating windows, 
//manage OpenGL contexts, and handle user input 

//GLAD is a library that dynamically loads OpenGL function addresses at runtime

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Callback for resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

Player player = Player();


int main() {

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

	Terrain terrain = Terrain(); 
	terrain.setupMesh(); 

	//Tell OpenGL the window size
	//First two parameters tell OpenGL that (0,0) is at the bottom left of the screen
	//Last two parameters tell OpenGL the width and height of the window
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shader("./vertexShader.vert", "./fragmentShader.frag");

	//Initialize texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load texture (grass)
	int width, height, nrChannels;
	unsigned char* data = stbi_load("grass.jpg", &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data); 

	glEnable(GL_DEPTH_TEST);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(70.0f), 800.0f / 600.0f, 0.1f, 10000000.0f);

	//Disable cursor and set cursor functionality
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	//Main program loop
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Process user input such as mouse and key input
		processInput(window, deltaTime);

		// --<>--<>--<> || RENDER STUFF || <>--<>--<>-- //

		//Clear the screen
		glClearColor(0.5f, 0.78f, 0.89f, 1.0f);

		//Use the shader program
		shader.use();

		terrain.draw();

		player.setDirection();

		//Initialize matrices
		glm::mat4 model = glm::mat4(1.0f); 
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(player.cameraPos, player.cameraPos + player.cameraFront, player.cameraUp);

		//Apply projections to the shader
		int modelLoc = glGetUniformLocation(shader.ID, "model"); 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 
		int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		//Bind the texture and VAO
		glBindTexture(GL_TEXTURE_2D, texture);

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