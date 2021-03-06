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
	protected:
		SceneObject *parent;
		vector<SceneObject*> children;

		vec3 position, rotation, size;
		
		mat4 ctm; // current transformation matrix

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

		// incremental modifiers for object attributes
		virtual void translate(vec3 translation);
		virtual void rotate(vec3 rotation);
		virtual void scale(vec3 scale);

		vec3 getPosition();
		vec3 getRotation();
		vec3 getScale();

		// returns the current transformation matrix for this object
		mat4 getTransformationMatrix();

		// creates the ctm
		virtual void createMatrix();

		// add child to list of children
		void addChild(SceneObject *child);

		// remove this object's parent and have the parent remove it from
		// it's children
		void removeParent();

		string getName();

	private:

		// set this scene object's parent
		void setParent(SceneObject *parent);

		// remove a specific child
		void removeChild(SceneObject *child);
};

#endif
