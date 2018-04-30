#include "cRigidBody.h"
#include "BulletCalculations.h"

cRigidBody::cRigidBody(const sRigidBodyDesc desc, iShape* shape)
{
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass = desc.Mass;

	bool isDynamic;

	btVector3 localInertia;

	btCollisionShape* colShape = 0;

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	eShapeType theType = shape->getShapeType();
	if (theType == SHAPE_PLANE)
	{
		isDynamic = false;
		mass = 0.0f;
		localInertia = toBtVec(glm::vec3(0, 0, 0));
		cPlaneShape* castShape = (cPlaneShape*)shape;
		colShape = castShape->getBulletShape();
	}
	else if (theType == SHAPE_SPHERE)
	{
		isDynamic = (mass != 0.f);
		localInertia = toBtVec(desc.Velocity);
		cSphereShape* castShape = (cSphereShape*)shape;
		colShape = castShape->getBulletShape();
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);
	}
	else if (theType == SHAPE_CUBE)
	{
		isDynamic = (mass != 0.f);
		localInertia = toBtVec(desc.Velocity);
		cCubeShape* castShape = (cCubeShape*)shape;
		colShape = castShape->getBulletShape();
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);
	}
	else if (theType == SHAPE_CONE)
	{
		isDynamic = (mass != 0.f);
		localInertia = toBtVec(desc.Velocity);
		cConeShape* castShape = (cConeShape*)shape;
		colShape = castShape->getBulletShape();
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);
	}

	startTransform.setOrigin(toBtVec(desc.Position));


	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	//info = rbInfo;
	bulletRigidBody = new btRigidBody(rbInfo);

	bulletRigidBody->setRestitution(0.9);
	//Stops the balls from rolling indefinitely
	if (theType == SHAPE_SPHERE)
	{
		bulletRigidBody->setRollingFriction(0.02);
	}

	mShape = shape;
	mPosition = desc.Position;
	lastSafePos = desc.Position;
	mVelocity = desc.Velocity;
	mRotation = desc.Rotation;
	mMass = desc.Mass;
}

cRigidBody::~cRigidBody()
{
	if (bulletRigidBody && bulletRigidBody->getMotionState())
	{
		delete bulletRigidBody->getMotionState();
	}
}

iShape* cRigidBody::getShape()
{
	return mShape;
}

void cRigidBody::getTransform(glm::mat4 &transform)
{
	btTransform worldTrans;
	bulletRigidBody->getMotionState()->getWorldTransform(worldTrans);
	ToGlm(worldTrans, transform);
}

void cRigidBody::getPosition(glm::vec3 &position)
{
	position = toGLMVec(bulletRigidBody->getCenterOfMassPosition());
}

void cRigidBody::getRotation(glm::vec3 &rotation)
{
	btTransform transform;
	bulletRigidBody->getMotionState()->getWorldTransform(transform);
	rotation = toGLMQuatToVec(transform.getRotation());
}

void cRigidBody::getLastSafePos(glm::vec3 &safePos)
{
	safePos = lastSafePos;
}

void cRigidBody::applyForce(glm::vec3 force)
{
	this->bulletRigidBody->activate(true);

	this->bulletRigidBody->applyCentralForce(toBtVec(force));
}