#include "SceneObject.h"
#include "vec.h"
#include "mat.h"

#include <string>
#include <iostream>
#include <algorithm>

using std::string;
using std::cout;
using std::endl;
using std::remove;

SceneObject::SceneObject(string objName):
	objName(objName),
	parent(NULL),
	position(0), rotation(0), scale(1)
	
{
	createMatrix();
}

SceneObject::SceneObject(string objName, vec3 position):
		objName(objName),
		parent(NULL),
		position(position),
		rotation(0),
		scale(1)
{
	createMatrix();
}

SceneObject::SceneObject(string objName, vec3 position, vec3 rotation,
						 vec3 scale):
		objName(objName),
		parent(NULL),
		position(position), rotation(rotation), scale(scale)
{
	createMatrix();
}

void SceneObject::setPosition(vec3 position)
{
	this->position = position;

	createMatrix();
}

void SceneObject::setRotation(vec3 rotation)
{
	this->rotation = rotation;

	createMatrix();
}

void SceneObject::setScale(vec3 scale)
{
	this->scale = scale;

	createMatrix();
}

vec3 SceneObject::getPosition()
{
	return position;
}

vec3 SceneObject::getRotation()
{
	return rotation;
}

vec3 SceneObject::getScale()
{
	return scale;
}

mat4 SceneObject::getTransformationMatrix()
{
	return ctm;
}

void SceneObject::addChild(SceneObject *child)
{
	children.push_back(child);
	child->setParent(this);
}

void SceneObject::removeChild(SceneObject *child)
{
	remove(children.begin(), children.end(), child);
}

void SceneObject::setParent(SceneObject *parent)
{
	this->parent = parent;
}

void SceneObject::removeParent()
{
	parent->removeChild(this);
	parent = NULL;
}

string SceneObject::getName()
{
  return objName;
}

void SceneObject::createMatrix()
{
	ctm = Angel::identity();

	// if we have a parent, we have to have this object's
	// coordinates be the local to the parent's coordinates
	// and we have to apply the parent's ctm to our ctm
	if (parent != NULL)
	{
		ctm = Translate(parent->position.x, 
				parent->position.y, parent->position.z);
		
		ctm = ctm * parent->getTransformationMatrix();
	}

	// create our ctm
	ctm = ctm * Translate(position.x, position.y, position.z);
	ctm = ctm * RotateX(rotation.x);
	ctm = ctm * RotateY(rotation.y);
	ctm = ctm * RotateZ(rotation.z);
	ctm = ctm * Scale(scale.x, scale.y, scale.z);
	
	// if we have a parent, go back to world coordinates
	if (parent != NULL)
	{
		ctm = ctm * Translate(-(parent->position.x), 
			-(parent->position.y), -(parent->position.z));
	}

	// tell children to update their matrices
	for (vector<SceneObject*>::iterator iter = children.begin();
			iter != children.end(); ++iter)
	{
		(*iter)->createMatrix();
	}
}

