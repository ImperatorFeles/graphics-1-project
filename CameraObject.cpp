#include "CameraObject.h"
#include "mat.h"

#include <iostream>

using std::cout;
using std::endl;

CameraObject::CameraObject(string objName, float moveSpeed, float rotSpeed):
	SceneObject(objName), xRotLock(false), 
	moveSpeed(moveSpeed), rotSpeed(rotSpeed)
	
{
}

CameraObject::CameraObject(string objName, float moveSpeed,  float rotSpeed,
		vec3 position, vec3 rotation):
	SceneObject(objName, position, rotation, vec3(0)),
	xRotLock(false), moveSpeed(moveSpeed), rotSpeed(rotSpeed)
{

}

void CameraObject::update()
{
	// update all the positions
	if (forward)
	{
		velocity.z = moveSpeed;
	}
	else if (backward)
	{
		velocity.z = -moveSpeed;
	}
	else
	{
		velocity.z = 0;
	}

	if (up && !planarLock)
	{
		velocity.y = -moveSpeed;
	}
	else if (down && !planarLock)
	{
		velocity.y = moveSpeed;
	}
	else
	{
		velocity.y = 0;
	}

	if (left)
	{
		velocity.x = moveSpeed;
	}
	else if (right)
	{
		velocity.x = -moveSpeed;
	}
	else
	{
		velocity.x = 0;
	}

	// refresh the matrix
	createMatrix();
}

void CameraObject::setMoveForward(bool forward)
{
	this->forward = forward;
}

void CameraObject::setMoveBackward(bool backward)
{
	this->backward = backward;
}

void CameraObject::setMoveUp(bool up)
{
	this->up = up;
}

void CameraObject::setMoveDown(bool down)
{
	this->down = down;
}

void CameraObject::setMoveLeft(bool left)
{
	this->left = left;
}

void CameraObject::setMoveRight(bool right)
{
	this->right = right;
}

void CameraObject::translate(vec3 translation)
{
	position.x += translation.x;
	position.y += translation.y;
	position.z -= translation.z;

	// tell children to update their position 
	for (vector<SceneObject*>::iterator iter = children.begin();
			iter != children.end(); ++iter)
	{
		(*iter)->translate(translation);
		(*iter)->createMatrix();
	}
}

void CameraObject::rotate(vec3 rotation)
{
	// lock the camera's up and down movement if we have to
	if ((xRotLock && this->rotation.x + rotation.x * rotSpeed < -90) ||
			(xRotLock && this->rotation.x + rotation.x * rotSpeed > 90))
	{
		rotation.x = 0;
	}

	this->rotation = this->rotation + rotation * rotSpeed;

	// tell children to update their position 
	for (vector<SceneObject*>::iterator iter = children.begin();
			iter != children.end(); ++iter)
	{
		(*iter)->rotate(rotation);
		(*iter)->createMatrix();
	}	
}

void CameraObject::setLockedXRot(bool lock)
{
	xRotLock = lock;
}

void CameraObject::lockToPlane(float height)
{
	planarLock = true;
	position.y = plane = height;
}

void CameraObject::unlockPlane()
{
	planarLock = false;
}

void CameraObject::createMatrix()
{
	ctm = Angel::identity();

	vec4 tempPos = vec4(position, 1);

	// update position based on rotation
	mat4 tempMat = Angel::identity();
	if (planarLock)
	{
		tempMat = tempMat * RotateY(-rotation.y);
	}
	else
	{
		tempMat = tempMat * RotateY(-rotation.y) * RotateX(-rotation.x);
	}
	tempPos = tempPos + tempMat * velocity;
	position.x = tempPos.x;
	position.y = tempPos.y;
	position.z = tempPos.z;

	// update matrix as usual
	ctm = ctm * RotateX(rotation.x) * RotateY(rotation.y) * RotateZ(rotation.z);
	ctm = ctm * Translate(tempPos.x, tempPos.y, tempPos.z);


	// tell children to update their matrices
	for (vector<SceneObject*>::iterator iter = children.begin();
			iter != children.end(); ++iter)
	{
		(*iter)->createMatrix();
	}
}
