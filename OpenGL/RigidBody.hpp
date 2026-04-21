#include <glm.hpp>

class RigidBody {
protected:
	glm::vec3 position;
	glm::vec4 orientation;
	float linearVel;
	float angularVel;
	float invMass;
	// Inverse Inertia Tensor
	// Force Accumulator
	// Torque Accumulator
};