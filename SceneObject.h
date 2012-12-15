#ifndef _SCENE_OBJECT
#define _SCENE_OBJECT

#include "vec.h"
#include "mat.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class SceneObject
{
	private:
		string objName; // name of the object as defined by the file
		SceneObject *parent;
		vector<SceneObject*> children;

	protected:
		vec3 position, rotation, scale;
		
		mat4 ctm; // current transformation matrix

		// creates the ctm
		virtual void createMatrix();

	public:
		// for creating an object where position, rotation, scale are unused
		SceneObject(string objName);

		// for creating an object where rotation and scale are unused
		SceneObject(string objName, vec3 position);

		SceneObject(string objName, vec3 position, vec3 rotation, vec3 scale);

		// modifiers for object attributes
		void setPosition(vec3 position);
		void setRotation(vec3 rotation);
		void setScale(vec3 scale);

		// returns the current transformation matrix for this object
		mat4 getTransformationMatrix();

		// add child to list of children
		void addChild(SceneObject *child);

		string getName();

	private:

		// set this scene object's parent
		void setParent(SceneObject *parent);
};

#endif
