#include "SceneObject.h"
#include "vec.h"
#include "mat.h"

#include <string>

using std::string;

SceneObject::SceneObject(string objName):
		objName(objName),
		position(0), rotation(0), scale(0)
{
	createMatrix();
}

SceneObject::SceneObject(string objName, vec3 position):
		objName(objName),
		position(position),
		rotation(0),
		scale(0)
{
	createMatrix();
}

SceneObject::SceneObject(string objName, vec3 position, vec3 rotation,
						 vec3 scale):
		objName(objName), position(position), rotation(rotation), scale(scale)
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

mat4 SceneObject::getTransformationMatrix()
{
	return ctm;
}

void SceneObject::createMatrix()
{
	ctm = Translate(position.x, position.y, position.z);
	ctm = ctm * RotateX(rotation.x);
	ctm = ctm * RotateY(rotation.y);
	ctm = ctm * RotateZ(rotation.z);
	ctm = ctm * Scale(scale.x, scale.y, scale.z);
}
