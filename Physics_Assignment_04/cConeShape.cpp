#include "cConeShape.h"

cConeShape::cConeShape(float radius, float height)
	:iShape(SHAPE_CONE)
{
	this->radius = radius;
	this->height = height;
	bulletCone = new btConeShape(radius, height);
}

cConeShape::~cConeShape()
{

}

bool cConeShape::coneGetRadiusHeight(float &radius, float &height)
{
	radius = this->radius;
	height = this->height;
	return true;
}

btCollisionShape* cConeShape::getBulletShape()
{
	return this->bulletCone;
}

