#ifndef _HG_cPlaneShape_
#define _HG_cPlaneShape_

#include <glm/vec3.hpp>
#include <btBulletDynamicsCommon.h>
#include "iShape.h"

class cPlaneShape : public iShape
{
public:
	cPlaneShape(glm::vec3 normal, float planeConst);

	virtual ~cPlaneShape();

	virtual bool planeGetNormal(glm::vec3 &normal);
	virtual bool planeGetPlaneConst(float &planeConst);

	btCollisionShape* getBulletShape();

private:
	glm::vec3 normalVec;
	float planeConst;
	btCollisionShape* bulletPlane;
};

#endif