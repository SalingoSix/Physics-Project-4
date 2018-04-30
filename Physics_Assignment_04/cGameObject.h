#ifndef _HG_cGameObject_
#define _HG_cGameObject_

#include "cRigidBody.h"
#include <string>

class cGameObject
{
public:
	cGameObject();
	~cGameObject();

	//The rigid body can handle all physical aspects like position and rotation
	cRigidBody* Body;
	//Which mesh to draw
	std::string MeshName;



};

#endif