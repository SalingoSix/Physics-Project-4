#ifndef _HG_cBulletSphereShape_
#define _HG_cBulletSphereShape_

#include <glm/vec3.hpp>
#include "iShape.h"
#include <btBulletDynamicsCommon.h>

class cSphereShape : public iShape
{
public:
	cSphereShape(float radius);

	virtual ~cSphereShape();

	virtual bool sphereGetRadius(float &radius);

	btCollisionShape* getBulletShape();

private:
	float mRadius;
	btCollisionShape* bulletSphere;
};

#endif