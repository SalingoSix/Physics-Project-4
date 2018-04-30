#ifndef _HG_cRigidBody_
#define _HG_cRigidBody_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <btBulletDynamicsCommon.h>

#include "cPlaneShape.h"
#include "cSphereShape.h"
#include "cCubeShape.h"
#include "cConeShape.h"


struct sRigidBodyDesc
{
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Velocity;
	glm::vec3 Accel;
	float Mass;

	sRigidBodyDesc()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		Accel = glm::vec3(0.0f, 0.0f, 0.0f);
		Mass = 1.0f;
	}
};

class cPhysicsWorld;
class cRigidBody
{
public:
	cRigidBody(const sRigidBodyDesc desc, iShape* shape);
	virtual ~cRigidBody();

	virtual iShape* getShape();

	virtual void getTransform(glm::mat4 &transform);
	virtual void getPosition(glm::vec3 &position);
	virtual void getRotation(glm::vec3 &rotation);
	virtual void getLastSafePos(glm::vec3 &safe);
	virtual void applyForce(glm::vec3 force);

private:
	friend class cPhysicsWorld;

	iShape* mShape;
	glm::vec3 mPosition;
	glm::vec3 lastSafePos;
	glm::vec3 mVelocity;
	glm::quat mRotation;
	glm::vec3 mAccel;
	glm::vec3 appliedForce;
	float mMass;

	btRigidBody::btRigidBodyConstructionInfo* info;
	btRigidBody* bulletRigidBody;
};

#endif