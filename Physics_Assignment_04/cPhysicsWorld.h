#ifndef _HG_cPhysicsWorld_
#define _HG_cPhysicsWorld_

#include <vector>
#include <btBulletDynamicsCommon.h>
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include "cRigidBody.h"

struct sRK4State
{
	sRK4State(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		Position = a;
		Velocity = b;
		Acceleration = c;
	}
	sRK4State()
	{
		Position = glm::vec3{ 0.0f };
		Velocity = glm::vec3{ 0.0f };
		Acceleration = glm::vec3{ 0.0f };
	}
	glm::vec3 Position;
	glm::vec3 Velocity;
	glm::vec3 Acceleration;
};

class cPhysicsWorld
{
public:

	cPhysicsWorld();

	~cPhysicsWorld();

	void TimeStep(float deltaTime);

	void AddRigidBody(cRigidBody* rigidBody);
	void RemoveRigidBody(cRigidBody* rigidBody);
	void PointToPointConstraint(cRigidBody* rigidBody, glm::vec3 HangPoint);
	void PointToPointConstraint(cRigidBody* rigidBodyA, cRigidBody* rigidBodyB, glm::vec3 HangPoint);
	void HingeConstraint(cRigidBody* rigidBody, glm::vec3 hingePos, glm::vec3 hingeAxis);
	void HingeConstraint(cRigidBody* rigidBodyA, cRigidBody* rigidBodyB, float hingeDisplaceX, float hingeDisplaceY, float hingeRotation);
	void SliderConstraint(cRigidBody* rigidBody, float sliderRotY, float sliderRotZ, float extentMax);
	void SliderConstraint(cRigidBody* rigidBodyA, cRigidBody* rigidBodyB, float sliderRotY, float sliderRotZ, float extentMax);
	void sixDofConstraint(cRigidBody* rigidBody, glm::vec3 constraintOrigin, glm::vec3 sliderExtents, glm::vec3 angularLimit);
private:
	btDiscreteDynamicsWorld* dynamicsWorld;
	std::vector<cRigidBody*> mVecRigidBodies;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	//For sound effect playing
	FMOD_RESULT result;
	FMOD::System* system = NULL;
	FMOD::Channel* channel;
	FMOD::Sound* sound;
	bool doWeHaveSound;
};

#endif