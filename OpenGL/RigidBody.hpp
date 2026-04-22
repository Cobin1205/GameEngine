#include <glm.hpp>
//#include <glm/gtc/quaternion.hpp>

class RigidBody {
protected:
	glm::vec3 position;
	//glm::quat orientation;

	glm::vec3 linearVel;
	glm::vec3 angularVel;

	float invMass;

	glm::mat3 inverseInertiaBody;
	glm::mat3 inverseInertiaWorld;

	glm::vec3 forceAccum;
	glm::vec3 torqueAccum;

public:
	void applyForce(const glm::vec3& force);
	void applyForceAtPoint(const glm::vec3& force, const glm::vec3 point);
	void applyTorque(const glm::vec3& torque);

	void integrate(float dt);
	void clearAccumulators();
};