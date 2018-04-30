#ifndef _HG_cCubeShape_
#define _HG_cCubeShape_

#include <glm/vec3.hpp>
#include "iShape.h"
#include <btBulletDynamicsCommon.h>

class cCubeShape : public iShape
{
public:
	cCubeShape(float halfSize);

	virtual ~cCubeShape();

	virtual bool cubeGetHalfSize(float &halfSize);

	btCollisionShape* getBulletShape();

private:
	float mHalfSize;
	btCollisionShape* bulletCube;
};

#endif