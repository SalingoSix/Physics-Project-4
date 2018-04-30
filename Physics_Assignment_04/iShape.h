#ifndef _HG_iShape_
#define _HG_iShape_

#include <glm/vec3.hpp>

enum eShapeType
{
	SHAPE_SPHERE,
	SHAPE_PLANE,
	SHAPE_CUBE,
	SHAPE_CONE
};

class iShape
{
public:
	virtual ~iShape() {};
	eShapeType getShapeType()
	{
		return shapeType;
	}

	virtual bool sphereGetRadius(float &radius)
	{
		return false;
	}

	virtual bool planeGetNormal(glm::vec3 &normal)
	{
		return false;
	}

	virtual bool planeGetPlaneConst(float &planeConst)
	{
		return false;
	}

	virtual bool cubeGetHalfSize(float &halfSize)
	{
		return false;
	}

	virtual bool coneGetRadiusHeight(float &radius, float &height)
	{
		return false;
	}

protected:
	iShape(eShapeType type)
	{
		shapeType = type;
	}

private:
	eShapeType shapeType;
};

#endif