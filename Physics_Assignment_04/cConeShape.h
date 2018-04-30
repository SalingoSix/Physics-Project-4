#ifndef _HG_cConeShape_
#define _HG_cConeShape_

#include <glm/vec3.hpp>
#include "iShape.h"
#include <btBulletDynamicsCommon.h>

class cConeShape : public iShape
{
public:
	cConeShape(float radius, float height);

	virtual ~cConeShape();

	virtual bool coneGetRadiusHeight(float &radius, float &height);

	btCollisionShape* getBulletShape();

private:
	float radius;
	float height;
	btCollisionShape* bulletCone;
};

#endif