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

	if (up)
	{
		velocity.y = -moveSpeed;
	}
	else if (down)
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

void CameraObject::addXRot(float theta)
{
	if ((xRotLock && rotation.x + theta * rotSpeed < -90) ||
			(xRotLock && rotation.x + theta * rotSpeed > 90))
	{
		return;
	}

	rotation.x += theta * rotSpeed;
	
	if (rotation.x > 360)
	{
		rotation.x -= 360;
	}
}

void CameraObject::addYRot(float theta)
{
	rotation.y += theta * rotSpeed;

	if (rotation.y > 360)
	{
		rotation.y -= 360;
	}
}

void CameraObject::addZRot(float theta)
{
	rotation.z += theta * rotSpeed;

	if (rotation.z > 360)
	{
		rotation.z -= 360;
	}
}

void CameraObject::setLockedXRot(bool lock)
{
	xRotLock = lock;
}

void CameraObject::createMatrix()
{
	// update position based on rotation
	mat4 tempMat = Angel::identity();
	vec4 tempPos = vec4(position, 1);
	tempMat = tempMat * RotateY(-rotation.y) * RotateX(-rotation.x);
	tempPos = tempPos + tempMat * velocity;
	position.x = tempPos.x;
	position.y = tempPos.y;
	position.z = tempPos.z;

	// update matrix as usual
	ctm = Angel::identity();
	ctm = ctm * RotateX(rotation.x) * RotateY(rotation.y) * RotateZ(rotation.z);
	ctm = ctm * Translate(position);

}
