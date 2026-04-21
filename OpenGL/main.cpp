#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include "player.h"

// GLFW is a framework for creating windows, 
//manage OpenGL contexts, and handle user input 

//GLAD is a library that dynamically loads OpenGL function addresses at runtime

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Callback for resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

float dot(const glm::vec2& g, float dx, float dy) {
	return g.x * dx + g.y * dy;
}

glm::vec2 gradient(int x, int y) {
	int h = x * 374761393 + y * 668265263;
	h = (h ^ (h >> 13)) & 7;

	switch (h) {
	case 0: return { 1.0f, 1.0f };
	case 1: return { -1.0f, 1.0f };
	case 2: return { 1.0f,-1.0f };
	case 3: return { -1.0f,-1.0f };
	case 4: return { 1.0f, 0.0f };
	case 5: return { -1.0f, 0.0f };
	case 6: return { 0.0f, 1.0f };
	case 7: return { 0.0f,-1.0f };
	}
	return { 0.0f,0.0f };
}

float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float perlin2D(float x, float y) {
	int x0 = (int)std::floor(x);
	int y0 = (int)std::floor(y);
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float sx = x - x0;
	float sy = y - y0;

	glm::vec2 g00 = gradient(x0, y0);
	glm::vec2 g10 = gradient(x1, y0);
	glm::vec2 g01 = gradient(x0, y1);
	glm::vec2 g11 = gradient(x1, y1);

	float n00 = dot(g00, sx, sy);
	float n10 = dot(g10, sx - 1, sy);
	float n01 = dot(g01, sx, sy - 1);
	float n11 = dot(g11, sx - 1, sy - 1);

	float u = fade(sx);
	float v = fade(sy);

	float nx0 = lerp(n00, n10, u);
	float nx1 = lerp(n01, n11, u);

	return lerp(nx0, nx1, v);
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

	//Tell OpenGL the window size
	//First two parameters tell OpenGL that (0,0) is at the bottom left of the screen
	//Last two parameters tell OpenGL the width and height of the window
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shader("./vertexShader.vert", "./fragmentShader.frag");

	float vertices[200000];

	int i = 0;
	//Generate Perlin Noise Terrain Here
	for (int x = -100; x < 100; x++) {
		for (int z = -100; z < 100; z++) {
			vertices[i] = (float)x * 100.0f;

			float xNorm, zNorm;
			xNorm = ((float)x + 100.0f) / 200.0f;
			zNorm = ((float) z + 100.0f) / 200.0f;
			vertices[i + 1] = perlin2D(xNorm * 10.0f, zNorm * 10.0f) * 1000.0f;

			vertices[i + 2] = (float)z * 100.0f;

			vertices[i + 3] = (float)xNorm * 75.0f;
			vertices[i + 4] = (float)zNorm * 75.0f;

			i += 5;
		}
	}

	//Algorithmically Build the indices
	const int SIZE = 200;
	std::vector<unsigned int> indices;

	for (int z = 0; z < SIZE - 1; z++) {
		for (int x = 0; x < SIZE - 1; x++) {

			int topLeft = z * SIZE + x;
			int topRight = topLeft + 1;
			int bottomLeft = (z + 1) * SIZE + x;
			int bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	//Initialize texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load texture (wood)
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

	// Initialize the buffer objects
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind the Array Object
	glBindVertexArray(VAO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Idk what any of this does ngl
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 

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
		glBindVertexArray(VAO);

		//draw
		glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	//Terminate GLFW
	glfwTerminate();

	return 0;
}

//Process user input such as player movement
void processInput(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	player.move(window, deltaTime, perlin2D);
}

//Do this each time the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	player.look(xpos, ypos);
}