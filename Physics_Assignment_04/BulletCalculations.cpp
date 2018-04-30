#include "BulletCalculations.h"

glm::vec3 crossProduct(glm::vec3 A, glm::vec3 B)
{
	glm::vec3 returnVec;
	returnVec.x = (A.y * B.z - A.z * B.y);
	returnVec.y = (A.z * B.x - A.x * B.z);
	returnVec.z = (A.x * B.y - A.y * B.x);

	return returnVec;
}

btVector3 toBtVec(glm::vec3 A)
{
	btVector3 returnVec;
	returnVec.setX(A.x);
	returnVec.setY(A.y);
	returnVec.setZ(A.z);
	return returnVec;
}

glm::vec3 toGLMVec(btVector3 A)
{
	glm::vec3 returnVec;
	returnVec.x = A.getX();
	returnVec.y = A.getY();
	returnVec.z = A.getZ();
	return returnVec;
}

glm::vec3 toGLMQuatToVec(btQuaternion A)
{
	float x, y, z;
	A.getEulerZYX(z, y, x);
	return glm::vec3(x, y, z);
}

void ToGlm(btTransform& transform, glm::mat4& out)
{
	transform.getOpenGLMatrix(&out[0][0]);
}

btTransform ToBullet(glm::mat4& mat)
{
	btTransform transform;
	transform.setFromOpenGLMatrix(&mat[0][0]);
	return transform;
}