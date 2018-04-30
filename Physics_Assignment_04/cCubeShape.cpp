#include "cCubeShape.h"

cCubeShape::cCubeShape(float halfSize)
	:iShape(SHAPE_CUBE)
{
	bulletCube = new btBoxShape(btVector3(halfSize, halfSize, halfSize));
	this->mHalfSize = halfSize;
}

cCubeShape::~cCubeShape()
{

}

bool cCubeShape::cubeGetHalfSize(float &halfSize)
{
	halfSize = this->mHalfSize;
	return true;
}

btCollisionShape* cCubeShape::getBulletShape()
{
	return this->bulletCube;
}