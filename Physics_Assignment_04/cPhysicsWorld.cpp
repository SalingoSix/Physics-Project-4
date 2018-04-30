#include "cPhysicsWorld.h"
#include "BulletCalculations.h"
#include <glm/gtc/matrix_transform.hpp>

cPhysicsWorld::cPhysicsWorld()
{
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	///-----initialization_end-----

	//FMOD Initialization
	doWeHaveSound = true;
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK)
	{
		doWeHaveSound = false;
		return;
	}

	result = system->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		doWeHaveSound = false;
		return;
	}
	
	//Load the sound now
	std::string soundEffect = "assets/sounds/Klonk.mp3";
	result = system->createSound(soundEffect.c_str(), FMOD_CREATESAMPLE, 0, &sound);
	if (result != FMOD_OK)
	{
		doWeHaveSound = false;
		return;
	}
}
cPhysicsWorld::~cPhysicsWorld()
{
	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;
}

void cPhysicsWorld::TimeStep(float deltaTime)
{

	dynamicsWorld->stepSimulation(deltaTime, 10);

	if (!doWeHaveSound)
		return;

	result = system->update();

	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i != numManifolds; i++)
	{
		btPersistentManifold* thisManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		//Check if either object in the manifold is non-moving (ie. a plane)
		const btCollisionObject* collisionObject = thisManifold->getBody0();
		const btCollisionShape* collisionShape = collisionObject->getCollisionShape();
		if (collisionShape->isNonMoving())
			continue;

		collisionObject = thisManifold->getBody1();
		collisionShape = collisionObject->getCollisionShape();
		if (collisionShape->isNonMoving())
			continue;

		int numContacts = thisManifold->getNumContacts();
		for (int j = 0; j != numContacts; j++)
		{	//If the two objects are touching, make the noise
			btManifoldPoint& pt = thisManifold->getContactPoint(j);
			if (pt.getDistance() <= 0.0f)
			{
				result = system->playSound(sound, 0, false, &channel);
				return;
			}
		}
	}
}

void cPhysicsWorld::AddRigidBody(cRigidBody* rigidBody)
{
	cRigidBody* castBody = (cRigidBody*)rigidBody;
	if (castBody == NULL)
		return;

	for (int index = 0; index < dynamicsWorld->getNumCollisionObjects(); index++)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[index];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body == castBody->bulletRigidBody)
			return;
	}

	dynamicsWorld->addRigidBody(castBody->bulletRigidBody);
	return;
}

void cPhysicsWorld::RemoveRigidBody(cRigidBody* rigidBody)
{
	cRigidBody* castBody = (cRigidBody*)rigidBody;
	if (castBody == NULL)
		return;

	for (int index = 0; index < dynamicsWorld->getNumCollisionObjects(); index++)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[index];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body == castBody->bulletRigidBody)
		{
			//dynamicsWorld->removeRigidBody(castBody->bulletRigidBody);
			dynamicsWorld->removeCollisionObject(obj);
			return;
		}
	}
	return;
}

//Connecting an object to a point in the world
void cPhysicsWorld::PointToPointConstraint(cRigidBody* rigidBody, glm::vec3 HangPoint)
{
	btVector3 pivotPointA = toBtVec(rigidBody->mPosition) + toBtVec(HangPoint);
	btPoint2PointConstraint* leftSpring = new btPoint2PointConstraint(*rigidBody->bulletRigidBody, pivotPointA);
	dynamicsWorld->addConstraint(leftSpring);
	return;
}

//Connecting two objects with a ball-socket connection
void cPhysicsWorld::PointToPointConstraint(cRigidBody* rigidBodyA, cRigidBody* rigidBodyB, glm::vec3 HangPoint)
{
	btTypedConstraint* P2PConstraint = new btPoint2PointConstraint(*rigidBodyA->bulletRigidBody, *rigidBodyB->bulletRigidBody, btVector3(0,0,0), toBtVec(HangPoint));
	dynamicsWorld->addConstraint(P2PConstraint);
	return;
}

//Hang a body by a point somewhere in space using a hinge
void cPhysicsWorld::HingeConstraint(cRigidBody* rigidBody, glm::vec3 hingePos, glm::vec3 hingeAxis)
{
	btVector3 pivotPointA = toBtVec(hingePos);
	btVector3 axisPlaneA = toBtVec(hingeAxis);
	btTypedConstraint* HingeConstraint = new btHingeConstraint(*rigidBody->bulletRigidBody, pivotPointA, axisPlaneA);
	dynamicsWorld->addConstraint(HingeConstraint);
	return;
}

//Connect two bodies to one another using a hinge
void cPhysicsWorld::HingeConstraint(cRigidBody* rigidBodyA, cRigidBody* rigidBodyB, float hingeDisplaceX, float hingeDisplaceY, float hingeRotation)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(hingeDisplaceX, hingeDisplaceY, 0.0f));
	transform = glm::rotate(transform, glm::radians(hingeRotation), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 identity(1.0f);
	btTransform identityBT = ToBullet(identity);

	btTypedConstraint* HingeConstraint = new btHingeConstraint(*rigidBodyA->bulletRigidBody, *rigidBodyB->bulletRigidBody, identityBT, ToBullet(transform));
	dynamicsWorld->addConstraint(HingeConstraint);

	return;
}

//Slider constraint with one object
void cPhysicsWorld::SliderConstraint(cRigidBody* rigidBody, float sliderRotY, float sliderRotZ, float extentMax)
{
	glm::mat4 transform(1.0f);
	transform = glm::rotate(transform, glm::radians(sliderRotY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(sliderRotZ), glm::vec3(0.0f, 0.0f, 1.0f));

	btSliderConstraint* SliderConstraint = new btSliderConstraint(*rigidBody->bulletRigidBody, ToBullet(transform), false);
	SliderConstraint->setUpperLinLimit(extentMax);
	SliderConstraint->setLowerLinLimit(-extentMax);
	dynamicsWorld->addConstraint(SliderConstraint);
}

//Slider constraint using two objects
void cPhysicsWorld::SliderConstraint(cRigidBody* rigidBodyA, cRigidBody* rigidBodyB, float sliderRotY, float sliderRotZ, float extentMax)
{
	glm::mat4 transform(1.0f);
	btTransform identity = ToBullet(transform);
	transform = glm::rotate(transform, glm::radians(sliderRotY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(sliderRotZ), glm::vec3(0.0f, 0.0f, 1.0f));

	btSliderConstraint* SliderConstraint = new btSliderConstraint(*rigidBodyA->bulletRigidBody, *rigidBodyB->bulletRigidBody, identity, ToBullet(transform), true);
	SliderConstraint->setUpperLinLimit(extentMax);
	SliderConstraint->setLowerLinLimit(-extentMax);
	dynamicsWorld->addConstraint(SliderConstraint);
}

//Upper and lower slider extents denote how far the object can slide along the given axes, or 0 to disable them. Angular extents are in degrees, 0 to disable
void cPhysicsWorld::sixDofConstraint(cRigidBody* rigidBody, glm::vec3 constraintOrigin, glm::vec3 sliderExtents, glm::vec3 angularLimit)
{
	btTransform frameInA, frameInB;
	frameInA = btTransform::getIdentity();
	frameInB = btTransform::getIdentity();
	frameInA.setOrigin(toBtVec(constraintOrigin));

	btGeneric6DofConstraint* myConstraint = new btGeneric6DofConstraint(*rigidBody->bulletRigidBody, frameInA, true);
	myConstraint->setLinearLowerLimit(toBtVec(sliderExtents));
	myConstraint->setLinearUpperLimit(toBtVec(-sliderExtents));

	glm::vec3 convertedLimits(glm::radians(angularLimit.x), glm::radians(angularLimit.y), glm::radians(angularLimit.z));
	myConstraint->setAngularLowerLimit(toBtVec(convertedLimits));
	myConstraint->setAngularUpperLimit(toBtVec(-convertedLimits));

	dynamicsWorld->addConstraint(myConstraint);
}