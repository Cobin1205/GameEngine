#pragma once
#include <glm.hpp>
#include <vector>
#include <glad/glad.h>
//#include <glm/gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "shader.h"
#include "stb_image.h"

class RigidBody {
protected:
	//Physics stuff
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	//glm::quat orientation;
	glm::vec3 linearVel = glm::vec3(0.0f);
	glm::vec3 angularVel = glm::vec3(0.0f);
	float invMass = 0.0f;
	glm::mat3 inverseInertiaBody = glm::mat3(0.0f);
	glm::mat3 inverseInertiaWorld = glm::mat3(0.0f);
	glm::vec3 forceAccum = glm::vec3(0.0f);
	glm::vec3 torqueAccum = glm::vec3(0.0f);

	//Style stuff
	glm::vec4 color = glm::vec4(0.0f);
	unsigned int textureID;
	std::string drawMode = "color";

	//Rendering stuff
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	unsigned int VBO, VAO, EBO;

public:

	void applyForce(const glm::vec3& force);
	void applyForceAtPoint(const glm::vec3& force, const glm::vec3 point);
	void applyTorque(const glm::vec3& torque);

	void integrate(float dt);
	void clearAccumulators();

	virtual void generateMesh() {
	
	};

	//Do weird OpenGL stuff
	void setupMesh() {
		// Initialize the buffer objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Bind the Array Object
		glBindVertexArray(VAO);
		//Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		//Bind EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


		GLsizei stride = 5 * sizeof(float);
		// Tell the vertex shader what to do with vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	//Get the matrix that describes position, rotation, and scale
	glm::mat4 getModelMatrix() {
		glm::mat4 model = glm::mat4(1.0f);

		//translate to position
		model = glm::translate(model, position);
		
		//rotate: (model, axis in radians, axis position)
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));

		//scale model by scale variable
		model = glm::scale(model, scale);

		return model;
	}

	//load image file and then use it as the texture	
	//Only do this if either 
	// 1. only this item uses this texture 
	// 2. you're going to use the return value to set the texture of other objects
	// Otherwise, it's really slow (reloading file for each object)
	int useTexture(std::string fileName) {
		drawMode = "texture";
		//Initialize texture
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set wrapping to CLAMP_TO_BORDER so it doesn't repeat the image
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// Set the border color to white to match your JPG background
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		//load texture (grass)
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load("TheTripleT.jpg", &width, &height, &nrChannels, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		return textureID;
	}

	void useTexture(unsigned int newID) {
		drawMode = "texture";
		textureID = newID;
	}

	void useColor(glm::vec4 newCol) {
		drawMode = "color";
		color = newCol;
	}

	void draw(Shader& shader) {
		glm::mat4 model = getModelMatrix();
		shader.setMat4("model", model);

		if (drawMode == "color") {
			shader.setBool("useTexture", 0);
			shader.setVec4("ourColor", color);
		}
		else {
			shader.setBool("useTexture", 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	}
};