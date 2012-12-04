#include "ModelObject.h"
#include "vec.h"

#include <vector>
#include <iostream>

using std::cout;
using std::vector;

ModelObject::ModelObject(string objName, vector<vec4> vertices, vector<vec3> normals, vector<vec2> textureUVs): 
		SceneObject(objName), vertices(vertices),
		normals(normals), textureUVs(textureUVs)
{
}

ModelObject::ModelObject(string objName, vector<vec4> vertices, vector<vec3> normals, vector<vec2> textureUVs, vec3 position, vec3 rotation, vec3 scale): 
		SceneObject(objName, position, rotation, scale), vertices(vertices),
		normals(normals), textureUVs(textureUVs)
{
}

vector<vec4> ModelObject::getVertices()
{
	return vertices;
}

vector<vec3> ModelObject::getNormals()
{
	return normals;
}

vector<vec2> ModelObject::getTextureUVs()
{
	return textureUVs;
}
