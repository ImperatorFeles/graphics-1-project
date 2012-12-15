#ifndef _LIGHT_OBJECT
#define _LIGHT_OBJECT

#include "SceneObject.h"
#include "vec.h"

#include <vector>
#include <cstring>
#include <cstdio>

using std::vector;
using std::memset;
using std::snprintf;

class LightObject;

vector<LightObject> getNearestLights(vector<LightObject> &lights, vec3 position);


struct LightInfo {
	GLint lightIndex;
	bool isSpotLight;
	
	struct {
		GLint ambient_id,
			  diffuse_id, 
			  specular_id,
			  position_id,
			  spotDirection_id,
			  spotExponent_id,
			  spotCutoff_id,
			  constantAttenuation_id,
			  linearAttenbuation_id,
			  quadraticAttenuation_id;
	} ids;
	struct {
		vec4 ambient,
			 diffuse,
			 specular,
			 spotDirection;
		float spotExponent,
			  spotCutoff,
			  constantAttenuation,
			  linearAttenbuation,
			  quadraticAttenuation;
	} values;
};

class LightObject : public SceneObject {
private:
	LightInfo info;

public:
	LightObject();
	LightObject(string objName, vec3 position, LightInfo info);

	void setLightIndex(GLint index);
	void bindLight(GLint program);
	void setValues();

};

#endif
