#include "cPlaneShape.h"
#include "BulletCalculations.h"

#include <glm/geometric.hpp>

cPlaneShape::cPlaneShape(glm::vec3 normal, float planeConst)
	: iShape(SHAPE_PLANE)
{
	btVector3 btNormal;

	normalVec = normal;
	this->planeConst = planeConst;

	btNormal = toBtVec(normalVec);
	bulletPlane = new btStaticPlaneShape(btNormal, planeConst);
}

cPlaneShape::~cPlaneShape()
{

}

bool cPlaneShape::planeGetNormal(glm::vec3 &normal)
{
	normal = this->normalVec;
	return true;
}

bool cPlaneShape::planeGetPlaneConst(float &planeConst)
{
	planeConst = this->planeConst;
	return true;
}

btCollisionShape* cPlaneShape::getBulletShape()
{
	return this->bulletPlane;
}