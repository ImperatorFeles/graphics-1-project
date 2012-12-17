#ifndef _LIGHT_OBJECT
#define _LIGHT_OBJECT

#include "SceneObject.h"
#include "vec.h"

#include <vector>
#include <cstring>
#include <cstdio>
#include <iostream> // TODO: Delete
using std::cout; using std::endl;

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

	void incAmbientValue(int num) {
		if (num == 1) info.values.diffuse += vec4(0.05, 0.0, 0.0, 0.0);
		if (num == 5) info.values.diffuse += vec4(-0.05, 0.0, 0.0, 0.0);
		if (num == 2) info.values.diffuse += vec4(0.0, 0.05, 0.0, 0.0);
		if (num == 6) info.values.diffuse += vec4(0.0, -0.05, 0.0, 0.0);
		if (num == 3) info.values.diffuse += vec4(0.0, 0.0, 0.05, 0.0);
		if (num == 7) info.values.diffuse += vec4(0.0, 0.0, -0.05, 0.0);
		if (num == 4) info.values.diffuse += vec4(0.0, 0.0, 0.0, 0.05);
		if (num == 8) info.values.diffuse += vec4(0.0, 0.0, 0.0, -0.05);
		cout <<       info.values.diffuse << endl;
	}
	vec4 getValue() { return info.values.specular; }
	void setLightIndex(GLint index);
	void bindLight(GLint program);
	void setValues();

	void translate(vec3 translation);

};

#endif
