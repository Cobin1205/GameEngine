#include <cmath>
#include <glm.hpp>
#include <vector>
#include <glad/glad.h>
#include "shader.h"

class Terrain {
private:

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	unsigned int VBO, VAO, EBO, texture;

	//Perlin Noise Functions
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

	// Implement adjustable size / resolution
	void generate() {
		//Generate the vertices
		int i = 0;
		for (int x = -100; x < 100; x++) {
			for (int z = -100; z < 100; z++) {
				vertices[i] = (float) x;

				float xNorm, zNorm;
				xNorm = ((float)x + 1.0f) / 2.0f;
				zNorm = ((float)z + 1.0f) / 2.0f;
				vertices[i + 1] = perlin2D(xNorm * 0.1f, zNorm * 0.1f) * 10.0f;

				vertices[i + 2] = (float)z;

				vertices[i + 3] = (float)xNorm * 0.75f;
				vertices[i + 4] = (float)zNorm * 0.75f;

				i += 5;
			}
		}

		//Algorithmically Build the indices
		const int SIZE = 200;

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
	}

public:
	Terrain() {
		//Initialize texture
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

		vertices.resize(200 * 200 * 5);
		generate();
	}

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

		// Idk what any of this does ngl
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void draw(Shader& shader) {
		shader.setMat4("model", glm::mat4(1.0f));
		shader.setBool("useTexture", 1);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	}

	float getHeight(float x, float z) {
		float xNorm = ((float)x + 100.0f) / 200.0f;
		float zNorm = ((float)z + 100.0f) / 200.0f;
		return perlin2D(xNorm * 10.0f, zNorm * 10.0f) * 1000.0f;
	}

};