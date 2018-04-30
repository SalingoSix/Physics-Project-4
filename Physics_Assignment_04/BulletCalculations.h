#ifndef _HG_BulletCalculations_
#define _HG_BulletCalculations_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <btBulletDynamicsCommon.h>

glm::vec3 crossProduct(glm::vec3 A, glm::vec3 B);

btVector3 toBtVec(glm::vec3 A);

glm::vec3 toGLMVec(btVector3 A);

glm::vec3 toGLMQuatToVec(btQuaternion A);

void ToGlm(btTransform& transform, glm::mat4& out);

btTransform ToBullet(glm::mat4& mat);

#endif