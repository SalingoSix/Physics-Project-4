#include "cSkinnedGameObject.h"
#include "cShaderProgram.h"

#include <stack>
#include <glm\gtc\matrix_transform.hpp>

cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = glm::vec3(0.0f);
	this->Scale = glm::vec3(1.0f);
	this->OrientationQuat = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	this->OrientationEuler = glm::vec3(0.0f);

	this->defaultAnimState = new cAnimationState();
	this->defaultAnimState->defaultAnimation.name = modelDir;
	this->defaultAnimState->defaultAnimation.frameStepTime = 0.005f;
	this->defaultAnimState->defaultAnimation.totalTime = this->Model->GetDuration();
}
cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;

	this->defaultAnimState = new cAnimationState();
	this->defaultAnimState->defaultAnimation.name = modelDir;
	this->defaultAnimState->defaultAnimation.frameStepTime = 0.005f;
	this->defaultAnimState->defaultAnimation.totalTime = this->Model->GetDuration();
}
cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, std::vector<std::string> charAnimations)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;

	this->defaultAnimState = new cAnimationState();
	this->defaultAnimState->defaultAnimation.name = modelDir;
	this->defaultAnimState->defaultAnimation.frameStepTime = 0.005f;
	this->defaultAnimState->defaultAnimation.totalTime = this->Model->GetDuration();

	this->vecCharacterAnimations = charAnimations;

	for (int i = 0; i != this->vecCharacterAnimations.size(); i++)
	{
		this->Model->LoadMeshAnimation(this->vecCharacterAnimations[i]);
	}

	this->animToPlay = this->defaultAnimState->defaultAnimation.name;
	this->TurnSpeed = 50.0f;
	this->CurrentSpeed = 1.0f;

}
cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, std::map<int, std::string> mapAnimations)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;

	this->defaultAnimState = new cAnimationState();
	this->defaultAnimState->defaultAnimation.name = modelDir;
	this->defaultAnimState->defaultAnimation.frameStepTime = 0.05f;
	this->defaultAnimState->defaultAnimation.totalTime = this->Model->GetDuration();

	this->curAnimState = new cAnimationState();
	this->curAnimState->defaultAnimation.name = modelDir;
	this->curAnimState->defaultAnimation.frameStepTime = 0.05f;
	this->curAnimState->defaultAnimation.totalTime = this->Model->GetDuration();

	this->mapCharacterAnimations = mapAnimations;

	std::map<int, std::string>::iterator mapIter = this->mapCharacterAnimations.begin();
	for (mapIter; mapIter != this->mapCharacterAnimations.end(); mapIter++)
	{
		this->Model->LoadMeshAnimation(mapIter->second);
	}
	this->TurnSpeed = 50.0f;
	this->Speed = 1.0f;
	this->animToPlay = this->defaultAnimState->defaultAnimation.name;

}
cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, float speed, std::map<int, std::string> mapAnimations)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;

	this->defaultAnimState = new cAnimationState();
	this->defaultAnimState->defaultAnimation.name = modelDir;
	this->defaultAnimState->defaultAnimation.frameStepTime = 0.05f;
	this->defaultAnimState->defaultAnimation.totalTime = this->Model->GetDuration();

	this->curAnimState = new cAnimationState();
	this->curAnimState->defaultAnimation.name = modelDir;
	this->curAnimState->defaultAnimation.frameStepTime = 0.05f;
	this->curAnimState->defaultAnimation.totalTime = this->Model->GetDuration();

	this->mapCharacterAnimations = mapAnimations;

	std::map<int, std::string>::iterator mapIter = this->mapCharacterAnimations.begin();
	for (mapIter; mapIter != this->mapCharacterAnimations.end(); mapIter++)
	{
		this->Model->LoadMeshAnimation(mapIter->second);
	}

	this->animToPlay = this->defaultAnimState->defaultAnimation.name;

	this->TurnSpeed = 50.0f;
	this->Speed = speed;
}
void cSkinnedGameObject::Move(float deltaTime)
{
	this->OrientationEuler.y += deltaTime * CurrentTurnSpeed;
	float distance = this->CurrentSpeed * deltaTime;
	float dx = distance * glm::sin(glm::radians(this->OrientationEuler.y));
	float dz = distance * glm::cos(glm::radians(this->OrientationEuler.y));
	this->Position += glm::vec3(dx, 0.0f, dz);
}

void cSkinnedGameObject::Draw(cShaderProgram Shader)
{
	//std::string animToPlay = "";
	float curFrameTime = 0.0f;


	if (this->defaultAnimState->defaultAnimation.name == this->animToPlay)
	{
		this->defaultAnimState->defaultAnimation.IncrementTime();
		curFrameTime = this->defaultAnimState->defaultAnimation.currentTime;
	}
	else
	{
		/*if (this->curAnimState->defaultAnimation.name != this->animToPlay)
			this->curAnimState->defaultAnimation.curTime = 0.0f;*/

		this->curAnimState->defaultAnimation.totalTime = this->Model->MapAnimationNameToScene[this->animToPlay]->mAnimations[0]->mDuration / 
		this->Model->MapAnimationNameToScene[this->animToPlay]->mAnimations[0]->mTicksPerSecond;
		this->curAnimState->defaultAnimation.name = this->animToPlay;
		this->curAnimState->defaultAnimation.IncrementTime();
		curFrameTime = this->curAnimState->defaultAnimation.currentTime;
	}

	std::vector<glm::mat4> vecFinalTransformation;
	std::vector<glm::mat4> vecOffsets;

	this->Model->BoneTransform(curFrameTime, animToPlay, vecFinalTransformation, this->vecBoneTransformation, vecOffsets);
	
	GLuint numBonesUsed = static_cast<GLuint>(vecFinalTransformation.size());
	glUseProgram(Shader.ID);
	Shader.setInt("numBonesUsed", numBonesUsed);
	glm::mat4* boneMatrixArray = &(vecFinalTransformation[0]);
	//glUniformMatrix4fv(glGetUniformLocation(Shader.ID, "Bones"), numBonesUsed, GL_FALSE, (const GLfloat*) glm::value_ptr(*boneMatrixArray));
	Shader.setMat4("bones", numBonesUsed, *boneMatrixArray);
	//Shader.setMat4("bones", numBonesUsed, vecFinalTransformation[0]);


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->Position);
	model = glm::rotate(model, glm::radians(this->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(this->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(this->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, this->Scale);
	Shader.setMat4("model", model);

	this->Model->Draw(Shader);
}