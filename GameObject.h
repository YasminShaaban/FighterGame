#pragma once

#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class GameObject{

public:
	int ID;
	glm::vec3 currentWorldPosition;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 currentMovingDirection;
	GLfloat Yaw;
	GLfloat Pitch;
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	glm::mat4 ModelMatrix;
	Model* mGameObjectModel;
	bool isVisible;
	glm::mat4 mBoundingSphereModelMatrix;
	glm::vec3 mInitialMovingDirection;
	glm::vec3 mInitialPosition;

	GameObject::GameObject(){
		this->ID = increment();
		this->currentWorldPosition = glm::vec3(0, 0, 0);
		this->currentMovingDirection = glm::vec3(0, 0, 1);
		this->MovementSpeed = 5.5f;
		this->Yaw = 90.0f;
		this->Pitch = 0.0f;
		this->isVisible = true;

	}
	//incrementing ID by one every time gameobject is created
	static int GameObject::increment(){
		static int counter = -1;
		counter++;
		return counter;
	}
	// function for setting parameters for any gameobject
	void GameObject::setParameters(glm::vec3 fMovingDirection, glm::vec3 fInitialPosition, float fMovingSpeed, Model* fModel)
	{
		//this->mInitialMovingDirection = glm::vec3(0, 0, 1);
		this->currentMovingDirection = glm::vec3(0, 0, 1);;
		//this->mInitialPosition = fInitialPosition;
		this->MovementSpeed = 6.5f;
		this->ModelMatrix = glm::mat4();
		this->mBoundingSphereModelMatrix = glm::mat4();
		this->mGameObjectModel = fModel;
		//this->Right = glm::normalize(glm::cross(this->mInitialMovingDirection, this->WorldUp));
	}
	// function to detect if two gameobjects intersect
	static bool DoObjectsIntersect(GameObject &object1, GameObject& object2)
	{
		if (object1.ID == object2.ID) return false;

		glm::vec3 object1_scale;
		glm::quat  object1_rotation;
		glm::vec3  object1_translation;
		glm::vec3  object1_skew;
		glm::vec4  object1_perspective;
		glm::decompose(object1.ModelMatrix, object1_scale, object1_rotation, object1_translation, object1_skew, object1_perspective);
		glm::vec3 object2_scale;
		glm::quat  object2_rotation;
		glm::vec3  object2_translation;
		glm::vec3  object2_skew;
		glm::vec4  object2_perspective;
		glm::decompose(object2.ModelMatrix, object2_scale, object2_rotation, object2_translation, object2_skew, object2_perspective);


		float distanceBetweenCenters = glm::distance(object1.ModelMatrix* glm::vec4(object1.mGameObjectModel->centre, 1),
			object2.ModelMatrix* glm::vec4(object2.mGameObjectModel->centre, 1));

		if (
			distanceBetweenCenters <
			(object1.mGameObjectModel->boundingSphereRadius*object1_scale[0] * .7) +
			(object2.mGameObjectModel->boundingSphereRadius*object2_scale[0] * .7)
			)
		{
			object2.isVisible = false;
			object1.isVisible = false;
			return true;

		}
		else
			return false;
	}

	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (direction == LEFT){
			this->currentMovingDirection = glm::vec3(0, 0, 1);
			this->currentWorldPosition += this->currentMovingDirection * velocity;

			if (this->currentWorldPosition.z > 10.0f)
				this->currentWorldPosition.z = 10.0f;
		}
		if (direction == RIGHT){
			this->currentMovingDirection = glm::vec3(0, 0, 1);
			this->currentWorldPosition -= this->currentMovingDirection * velocity;
			if (this->currentWorldPosition.z < -10.0f)
				this->currentWorldPosition.z = -10.0f;
		}
		if (direction == DOWNWARD)
		{
			this->currentMovingDirection = glm::vec3(0, 1, 0);

			this->currentWorldPosition -= this->currentMovingDirection * velocity;

			if (this->currentWorldPosition.y < -3.0f)
				this->currentWorldPosition.y = -3.0f;
		}
		if (direction == UPWARD)
		{
			this->currentMovingDirection = glm::vec3(0, 1, 0);
			this->currentWorldPosition += this->currentMovingDirection * velocity;

			if (this->currentWorldPosition.y > 3.0f)
				this->currentWorldPosition.y = 3.0f;

		}
		if (direction == BACKWARD)
		{
			this->currentMovingDirection = glm::vec3(1, 0, 0);

			this->currentWorldPosition += this->currentMovingDirection * velocity;

			if (this->currentWorldPosition.x > 5.0f)
				this->currentWorldPosition.x = 5.0f;
		}
		if (direction == FORWARD)
		{
			this->currentMovingDirection = glm::vec3(1, 0, 0);

			this->currentWorldPosition -= this->currentMovingDirection * velocity;
			if (this->currentWorldPosition.x < -3.0f)
			this->currentWorldPosition.x = -3.0f;

		}
		if (direction == FIRE)
		{
			this->currentMovingDirection = glm::vec3(1, 0, 0);
			this->currentWorldPosition -= this->currentMovingDirection * velocity*25.0f;
		}
	}
	//function to translate any gameobject taking translation vector glm::vec3 as parameter
	void translateBy(glm::vec3 fTranslationVector)
	{
		this->ModelMatrix = glm::translate(this->ModelMatrix, fTranslationVector);
	}
	//function to translate Bounding sphere of gameobject taking translation vector glm::vec3 as parameter
	void translateBoundingSphereBy(glm::vec3 fTranslationVector)
	{
		this->mBoundingSphereModelMatrix = glm::translate(this->mBoundingSphereModelMatrix, fTranslationVector);
	}
	//function to rotate any gameobject taking rotation vector glm::vec3 and radian angle  as parameters
	void rotateByAround(float radianAngle, glm::vec3 fRotationAxis)
	{
		this->ModelMatrix = glm::rotate(this->ModelMatrix, radianAngle, fRotationAxis);
	}
	//function to rotate Bounding sphere of gameobject taking rotation vector glm::vec3 and radian angle  as parameters
	void rotateBoundingSphereByAround(float radianAngle, glm::vec3 fRotationAxis)
	{
		this->mBoundingSphereModelMatrix = glm::rotate(this->mBoundingSphereModelMatrix, radianAngle, fRotationAxis);
	}
	//function to scale any gameobject taking scaling vector glm::vec3 as parameter
	void scaleBy(glm::vec3 scaleVector)
	{
		this->ModelMatrix = glm::scale(this->ModelMatrix, scaleVector);
	}
	//function to scale Bounding sphere of gameobject taking scaling vector glm::vec3 as parameter
	void scaleBoundingSphereBy(glm::vec3 scaleVector)
	{
		this->mBoundingSphereModelMatrix = glm::scale(this->mBoundingSphereModelMatrix, scaleVector);
	}


};
