#include "cSphereShape.h"

cSphereShape::cSphereShape(float radius)
	:iShape(SHAPE_SPHERE)
{
	bulletSphere = new btSphereShape(radius);
	this->mRadius = radius;
}

cSphereShape::~cSphereShape()
{

}

bool cSphereShape::sphereGetRadius(float &radius)
{
	radius = this->mRadius;
	return true;
}

btCollisionShape* cSphereShape::getBulletShape()
{
	return this->bulletSphere;
}