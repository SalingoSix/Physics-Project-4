#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "cShaderProgram.h"
#include "cCamera.h"
#include "cMesh.h"
#include "cModel.h"
#include "cSkybox.h"
#include "cSkinnedMesh.h"
#include "cSkinnedGameObject.h"
#include "cAnimationState.h"
#include "cScreenQuad.h"
#include "cPlaneObject.h"
#include "cFrameBuffer.h"
#include "cTexture.h"
#include "cGameObject.h"

//FOR PHYSICS ASSIGNMENT 4
#include "Bullet/btBulletDynamicsCommon.h"
#include "cPhysicsWorld.h"
#include "iShape.h"
#include "cSphereShape.h"
#include "cPlaneShape.h"
#include "cCubeShape.h"
#include "cConeShape.h"
#include "cRigidBody.h"

//Setting up a camera GLOBAL
cCamera Camera(glm::vec3(0.0f, 5.0f, 15.0f),		//Camera Position
			glm::vec3(0.0f, 1.0f, 0.0f),		//World Up vector
			0.0f,								//Pitch
			-90.0f);							//Yaw

float cameraRotAngle = 0.0f;
glm::vec3 forwardCamera;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = 400, lastY = 300;

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

int drawType = 1;

glm::vec3 roverPos = glm::vec3(0.0f);

std::map<std::string, cModel*> mapModelsToNames;
std::map<std::string, cSkinnedMesh*> mapSkinnedMeshToNames;
std::map<std::string, cShaderProgram*> mapShaderToName;

//FOR PHYSICS ASSIGNMENT 4
cPhysicsWorld* myPhysicsWorld = new cPhysicsWorld();
std::vector<cGameObject*> vecGameObjects;
std::vector<glm::vec3> vecConstraintPoints;
std::vector<std::pair<glm::vec3, glm::vec3>> vecJoints;
void addHingeConstraint(int rigidBodyIndex, glm::vec3 hingePos, glm::vec3 hingeAxis);
void addHingeConstraint(int index1, int index2, float xDisplace, float yDisplace, float hingeRotation);
void addPoint2PointConstraint(int rigidBodyIndex, glm::vec3 hangPoint);
void addPoint2PointConstraint(int index1, int index2, glm::vec3 hangPoint);
void addSliderConstraint(int rigidBodyIndex, float yRotation, float zRotation, float sliderExtents);
void addSliderConstraint(int index1, int index2, float yRotation, float zRotation, float sliderExtents);
void add6DofConstraint(int rigidBodyIndex, glm::vec3 constraintOrigin, glm::vec3 sliderExtents, glm::vec3 angularLimits);

void addPlane(glm::vec3 normal, float planeConst);
void addSphere(glm::vec3 position, float radius);
void addCube(glm::vec3 position, float halfSize);
void addCone(glm::vec3 position, float radius, float height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
	glfwInit();

	srand(time(NULL));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialzed GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//Setting up global openGL state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Set up all our programs
	cShaderProgram* myProgram = new cShaderProgram();
	myProgram->compileProgram("assets/shaders/", "vertShader.glsl", "fragShader.glsl");
	mapShaderToName["mainProgram"] = myProgram;

	myProgram = new cShaderProgram();
	myProgram->compileProgram("assets/shaders/", "animVert.glsl", "animFrag.glsl");
	mapShaderToName["skinProgram"] = myProgram;

	myProgram = new cShaderProgram();
	myProgram->compileProgram("assets/shaders/", "skyBoxVert.glsl", "skyBoxFrag.glsl");
	mapShaderToName["skyboxProgram"] = myProgram;

	myProgram = new cShaderProgram();
	myProgram->compileProgram("assets/shaders/", "modelVert.glsl", "modelFrag.glsl");
	mapShaderToName["simpleProgram"] = myProgram;

	myProgram = new cShaderProgram();
	myProgram->compileProgram("assets/shaders/", "quadVert.glsl", "quadFrag.glsl");
	mapShaderToName["quadProgram"] = myProgram;


	//Assemble all our models
	std::string path = "assets/models/sphere/sphere.obj";
	mapModelsToNames["Sphere"] = new cModel(path);

	path = "assets/models/floor/floor.obj";
	mapModelsToNames["Plane"] = new cModel(path);

	path = "assets/models/cube/cube.obj";
	mapModelsToNames["Cube"] = new cModel(path);

	path = "assets/models/cone/cone.obj";
	mapModelsToNames["Cone"] = new cModel(path);


	//Load various textures...
	cTexture staticTexture("assets/textures/static_texture_by_rachaelwrites.png");

	//Making a frame buffer to display the scene on a single quad
	cFrameBuffer mainFrameBuffer(SCR_HEIGHT, SCR_WIDTH);

	//Some simple shapes
	cScreenQuad screenQuad;

	//Positions for some of the point light
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -8.0f)
	};

	//Load the skyboxes
	cSkybox daybox("assets/textures/skybox/");
	cSkybox skybox("assets/textures/spacebox/");

	//Create all our rigid bodies / game objects

	//Two walls and a floor
	addPlane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
	addPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 10.0f);
	addPlane(glm::vec3(1.0f, 0.0f, 0.0f), 10.0f);

	//Members of the dangling P2P constraint
	addSphere(glm::vec3(0.5f, 5.0, -3.0f), 1.0f);
	addSphere(glm::vec3(0.0f, 10.0f, -3.0f), 1.0f);
	addCube(glm::vec3(0.0f, 7.5, -3.0f), 0.5f);
	addCone(glm::vec3(0.0f, 2.5f, -3.0f), 1.0f, 2.0f);

	addPoint2PointConstraint(4, glm::vec3(0.0f, 2.5f, 4.0f));
	addPoint2PointConstraint(4, 5, glm::vec3(0.0f, 2.5f, 0.0f));
	addPoint2PointConstraint(5, 3, glm::vec3(0.0f, 2.5f, 0.0f));
	addPoint2PointConstraint(3, 6, glm::vec3(0.0f, 2.5f, 0.0f));

	//Hinge constraint example
	addCube(glm::vec3(3.0f, 0.5f, 3.0f), 0.5f);
	addCube(glm::vec3(3.0f, 1.6f, 3.0f), 0.5f);

	addHingeConstraint(7, glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	addHingeConstraint(8, glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//Slider constraint example
	addSphere(glm::vec3(1.1f, 1.0f, -10.0f), 1.0f);
	addSphere(glm::vec3(-1.1f, 1.0f, -10.0f), 1.0f);
	addSphere(glm::vec3(0.0f, 1.0f, -8.0f), 1.0f);

	addSliderConstraint(9, 0.0f, 0.0f, 8.0f);
	addSliderConstraint(10, 0.0f, 0.0f, 8.0f);
	addSliderConstraint(11, 90.0f, 0.0f, 5.0f);

	//6 Degrees of Freedom example
	addCone(glm::vec3(-5.0f, 1.5f, 5.0f), 1.0f, 2.0f);

	add6DofConstraint(12, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f));

	//This sphere will be controllable, with the intent to push aroud other objects
	addSphere(glm::vec3(0.0f, 1.0f, 10.0f), 1.0f);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mapShaderToName["skyboxProgram"]->useProgram();
	mapShaderToName["skyboxProgram"]->setInt("skybox", 0);

	mapShaderToName["quadProgram"]->useProgram();
	mapShaderToName["quadProgram"]->setInt("screenTexture", 0);

	mapShaderToName["simpleProgram"]->useProgram();
	mapShaderToName["simpleProgram"]->setInt("texture_diffuse1", 0);
	mapShaderToName["simpleProgram"]->setInt("texture_static", 1);

	mapShaderToName["mainProgram"]->useProgram();
	mapShaderToName["mainProgram"]->setInt("skybox", 0);
	mapShaderToName["mainProgram"]->setInt("reflectRefract", 0);
	//Light settings go in here until I've made a class for them
	{
		//http://devernay.free.fr/cours/opengl/materials.html
		mapShaderToName["mainProgram"]->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		mapShaderToName["mainProgram"]->setVec3("dirLight.ambient", 0.15f, 0.15f, 0.15f);
		mapShaderToName["mainProgram"]->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		mapShaderToName["mainProgram"]->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		mapShaderToName["mainProgram"]->setVec3("pointLights[0].position", pointLightPositions[0]);
		mapShaderToName["mainProgram"]->setFloat("pointLights[0].constant", 1.0f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[0].linear", 0.09f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[0].quadratic", 0.032f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

		mapShaderToName["mainProgram"]->setVec3("pointLights[1].position", pointLightPositions[1]);
		mapShaderToName["mainProgram"]->setFloat("pointLights[1].constant", 1.0f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[1].linear", 0.09f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[1].quadratic", 0.032f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

		mapShaderToName["mainProgram"]->setVec3("pointLights[2].position", pointLightPositions[2]);
		mapShaderToName["mainProgram"]->setFloat("pointLights[2].constant", 1.0f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[2].linear", 0.09f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[2].quadratic", 0.032f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);

		mapShaderToName["mainProgram"]->setVec3("pointLights[3].position", pointLightPositions[3]);
		mapShaderToName["mainProgram"]->setFloat("pointLights[3].constant", 1.0f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[3].linear", 0.09f);
		mapShaderToName["mainProgram"]->setFloat("pointLights[3].quadratic", 0.032f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		mapShaderToName["mainProgram"]->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);

		mapShaderToName["mainProgram"]->setVec3("spotLight.position", glm::vec3(0.0f, 6.0f, 0.0f));
		mapShaderToName["mainProgram"]->setVec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
		mapShaderToName["mainProgram"]->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		mapShaderToName["mainProgram"]->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		mapShaderToName["mainProgram"]->setFloat("spotLight.constant", 1.0f);
		mapShaderToName["mainProgram"]->setFloat("spotLight.linear", 0.09f);
		mapShaderToName["mainProgram"]->setFloat("spotLight.quadratic", 0.032f);
		mapShaderToName["mainProgram"]->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		mapShaderToName["mainProgram"]->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		mapShaderToName["mainProgram"]->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		//http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	}

	float sceneTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		myPhysicsWorld->TimeStep(deltaTime);

		glm::mat4 projection = glm::perspective(glm::radians(Camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = Camera.getViewMatrix();
		glm::mat4 skyboxView = glm::mat4(glm::mat3(Camera.getViewMatrix()));

		glm::mat4 model(1.0f);

		//Begin writing to the main frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, mainFrameBuffer.FBO);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw all our world objects (spheres, planes, etc.)
		mapShaderToName["mainProgram"]->useProgram();
		mapShaderToName["mainProgram"]->setMat4("projection", projection);
		mapShaderToName["mainProgram"]->setMat4("view", view);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (int i = 0; i < vecGameObjects.size(); i++)
		{
			glm::mat4 myModel(1.0f);
			
			if (vecGameObjects[i]->MeshName == "Plane")
			{
				glm::vec3 normal;
				float planeConst;
				cPlaneShape* thisShape = (cPlaneShape*)vecGameObjects[i]->Body->getShape();
				thisShape->planeGetNormal(normal);
				thisShape->planeGetPlaneConst(planeConst);
				myModel = glm::translate(myModel, normal * planeConst);
				if (normal.y != 1.0f)
					myModel = glm::rotate(myModel, glm::radians(90.0f), glm::vec3(normal.z, 0.0f, normal.x));
				myModel = glm::scale(myModel, glm::vec3(10.0f));
			}
			else if (vecGameObjects[i]->MeshName == "Cone")
			{
				float radius, height;
				cConeShape* thisShape = (cConeShape*)vecGameObjects[i]->Body->getShape();
				thisShape->coneGetRadiusHeight(radius, height);
				vecGameObjects[i]->Body->getTransform(myModel);
				myModel = glm::scale(myModel, glm::vec3(1.0f, height / 2.0f, 1.0f));
			}
			else
			{
				vecGameObjects[i]->Body->getTransform(myModel);
			}
			mapShaderToName["mainProgram"]->setMat4("model", myModel);
			mapModelsToNames[vecGameObjects[i]->MeshName]->Draw(*mapShaderToName["mainProgram"]);
		}

		//Draw debug spheres representing the pivot points hung in space
		mapShaderToName["simpleProgram"]->useProgram();
		mapShaderToName["simpleProgram"]->setMat4("projection", projection);
		mapShaderToName["simpleProgram"]->setMat4("view", view);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int i = 0; i < vecConstraintPoints.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, vecConstraintPoints[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			mapShaderToName["simpleProgram"]->setMat4("model", model);
			mapModelsToNames["Sphere"]->Draw(*mapShaderToName["simpleProgram"]);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mapShaderToName["skyboxProgram"]->useProgram();

		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		mapShaderToName["skyboxProgram"]->setMat4("projection", projection);
		mapShaderToName["skyboxProgram"]->setMat4("view", skyboxView);

		glBindVertexArray(skybox.VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, daybox.textureID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);

		//Final pass: Render all of the above on one quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Paste the entire scene onto a quad as a single texture
		mapShaderToName["quadProgram"]->useProgram();
		mapShaderToName["quadProgram"]->setInt("drawType", drawType);
		glBindVertexArray(screenQuad.VAO);
		glBindTexture(GL_TEXTURE_2D, mainFrameBuffer.textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	return;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		drawType = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		drawType = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		drawType = 3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		drawType = 4;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		drawType = 5;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Camera.processKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Camera.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Camera.processKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Camera.processKeyboard(Camera_Movement::RIGHT, deltaTime);

	glm::vec3 appliedForce(0.0f);
	bool applyTheForce = false;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		appliedForce.z = -3.0f;
		applyTheForce = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		appliedForce.z = 3.0f;
		applyTheForce = true;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		appliedForce.x = -3.0f;
		applyTheForce = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		appliedForce.x = 3.0f;
		applyTheForce = true;
	}

	if (applyTheForce)
	{
		//Always create the moveable sphere at the end, so I don't need to change this number
		vecGameObjects[vecGameObjects.size() - 1]->Body->applyForce(appliedForce);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // this bool variable is initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	Camera.processMouseMovement(xOffset, yOffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera.processMouseScroll(yoffset);
}

void addHingeConstraint(int rigidBodyIndex, glm::vec3 hingePos, glm::vec3 hingeAxis)
{
	myPhysicsWorld->HingeConstraint(vecGameObjects[rigidBodyIndex]->Body, hingePos, hingeAxis);
	glm::vec3 bodyPos;
	vecGameObjects[rigidBodyIndex]->Body->getPosition(bodyPos);
	bodyPos += hingePos;
	vecConstraintPoints.push_back(bodyPos);
}

void addHingeConstraint(int index1, int index2, float xDisplace, float yDisplace, float hingeRotation)
{
	myPhysicsWorld->HingeConstraint(vecGameObjects[index1]->Body, vecGameObjects[index2]->Body, xDisplace, yDisplace, hingeRotation);
}

void addPoint2PointConstraint(int rigidBodyIndex, glm::vec3 hangPoint)
{
	myPhysicsWorld->PointToPointConstraint(vecGameObjects[rigidBodyIndex]->Body, hangPoint);
	glm::vec3 bodyPos;
	vecGameObjects[rigidBodyIndex]->Body->getPosition(bodyPos);
	bodyPos += hangPoint;
	vecConstraintPoints.push_back(bodyPos);
}

void addPoint2PointConstraint(int index1, int index2, glm::vec3 hangPoint)
{
	myPhysicsWorld->PointToPointConstraint(vecGameObjects[index1]->Body, vecGameObjects[index2]->Body, hangPoint);
}

void addSliderConstraint(int rigidBodyIndex, float yRotation, float zRotation, float sliderExtents)
{
	myPhysicsWorld->SliderConstraint(vecGameObjects[rigidBodyIndex]->Body, yRotation, zRotation, sliderExtents);
	glm::vec3 bodyPos;
	vecGameObjects[rigidBodyIndex]->Body->getPosition(bodyPos);
	vecConstraintPoints.push_back(bodyPos);
}

void addSliderConstraint(int index1, int index2, float yRotation, float zRotation, float sliderExtents)
{
	myPhysicsWorld->SliderConstraint(vecGameObjects[index1]->Body, vecGameObjects[index2]->Body, yRotation, zRotation, sliderExtents);
}

void add6DofConstraint(int rigidBodyIndex, glm::vec3 constraintOrigin, glm::vec3 sliderExtents, glm::vec3 angularLimits)
{
	myPhysicsWorld->sixDofConstraint(vecGameObjects[rigidBodyIndex]->Body, constraintOrigin, sliderExtents, angularLimits);
}

void addPlane(glm::vec3 normal, float planeConst)
{
	cPlaneShape* thisPlane = new cPlaneShape(normal, -planeConst);
	sRigidBodyDesc* theDesc = new sRigidBodyDesc();
	cRigidBody* theRigidBody = new cRigidBody(*theDesc, thisPlane);
	cGameObject* newObject = new cGameObject();
	newObject->Body = theRigidBody;
	newObject->MeshName = "Plane";
	vecGameObjects.push_back(newObject);
	myPhysicsWorld->AddRigidBody(theRigidBody);
}

void addSphere(glm::vec3 position, float radius)
{
	cSphereShape* theBall = new cSphereShape(radius);
	sRigidBodyDesc* theDesc = new sRigidBodyDesc();
	theDesc->Position = position;
	cRigidBody* theRigidBody = new cRigidBody(*theDesc, theBall);
	cGameObject* newObject = new cGameObject();
	newObject->Body = theRigidBody;
	newObject->MeshName = "Sphere";
	vecGameObjects.push_back(newObject);
	myPhysicsWorld->AddRigidBody(theRigidBody);
}

void addCube(glm::vec3 position, float halfSize)
{
	cCubeShape* theCube = new cCubeShape(halfSize);
	sRigidBodyDesc* theDesc = new sRigidBodyDesc();
	theDesc->Position = position;
	cRigidBody* theRigidBody = new cRigidBody(*theDesc, theCube);
	cGameObject* newObject = new cGameObject();
	newObject->Body = theRigidBody;
	newObject->MeshName = "Cube";
	vecGameObjects.push_back(newObject);
	myPhysicsWorld->AddRigidBody(theRigidBody);
}

void addCone(glm::vec3 position, float radius, float height)
{
	cConeShape* theCone = new cConeShape(radius, height);
	sRigidBodyDesc* theDesc = new sRigidBodyDesc();
	theDesc->Position = position;
	cRigidBody* theRigidBody = new cRigidBody(*theDesc, theCone);
	cGameObject* newObject = new cGameObject();
	newObject->Body = theRigidBody;
	newObject->MeshName = "Cone";
	vecGameObjects.push_back(newObject);
	myPhysicsWorld->AddRigidBody(theRigidBody);
}