#pragma once
#include "RigidBody.h"
#include "Shader.h"
#include <glm.hpp>

class Pyramid : public RigidBody {
public:
	Pyramid() {
		generateMesh();
		setupMesh();

		color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	void setPosition(glm::vec3 newPos) {
		position = newPos;
	}

	void rotate(glm::vec3 rot) {
		rotation = rotation + rot;
	}

	void setRotation(glm::vec3 newRot) {
		rotation = newRot;
	}

	void setScale(glm::vec3 factor) {
		scale = factor;
	}

	void addScale(glm::vec3 factor) {
		scale = scale + factor;
	}

    void generateMesh() override {
        //Generated with Gemini to save me from a headache
        // We are going to define each face as its own set of 3 vertices
        vertices = {
            // --- FRONT FACE ---
            // Position           // UV
             0.0f,  2.0f,  0.0f,   0.5f, 1.0f, // Top (Peak)
            -1.0f,  0.0f,  1.0f,   -1.0f, 0.0f, // Bottom Left
             1.0f,  0.0f,  1.0f,   2.0f, 0.0f, // Bottom Right

             // --- RIGHT FACE ---
              0.0f,  2.0f,  0.0f,   0.5f, 1.0f, // Top (Peak)
              1.0f,  0.0f,  1.0f,   -1.0f, 0.0f, // Bottom Left
              1.0f,  0.0f, -1.0f,   2.0f, 0.0f, // Bottom Right

              // --- BACK FACE ---
               0.0f,  2.0f,  0.0f,   0.5f, 1.0f, // Top (Peak)
               1.0f,  0.0f, -1.0f,   -1.0f, 0.0f, // Bottom Left
              -1.0f,  0.0f, -1.0f,   2.0f, 0.0f, // Bottom Right

              // --- LEFT FACE ---
               0.0f,  2.0f,  0.0f,   0.5f, 1.0f, // Top (Peak)
              -1.0f,  0.0f, -1.0f,   -1.0f, 0.0f, // Bottom Left
              -1.0f,  0.0f,  1.0f,   2.0f, 0.0f  // Bottom Right
        };

        // Now indices just point to these groups of 3
        indices = {
            0, 1, 2,    // Front
            3, 4, 5,    // Right
            6, 7, 8,    // Back
            9, 10, 11   // Left
        };
    }

	void setupMesh() {
		RigidBody::setupMesh();
	}

	
};