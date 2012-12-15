#ifndef _LIGHT_OBJECT
#define _LIGHT_OBJECT

#include "SceneObject.h"
#include "vec.h"

#include <vector>

using std::vector;

class LightObject;
class SpotLight;
class PointLight;

/**
 * This function returns up to 8 lights that affects the position
 * the most. 
 *
 * Normallly this would he the nearest 8 lights, however
 * the computation changes when the light is a spot light.
 * The returned vector is also sorted by how much it affects the
 * postion.
 *
 * @arg lights - A std::vector of light objects
 * @arg position - The point to test from.
 */
vector<LightObject> getNearestLights(vector<LightObject> &lights, vec3 position);

enum LightType { SPOT_LIGHT, POINT_LIGHT };

/** 
 * Light Object is a virtual class that extends SceneObject and
 * contains the bare minimum to specify a light source.
 * The class is virtual because there exist multiple types of 
 * lights: ambient, point, distant, and spotlight, which have 
 * additional properties. All lights will implement the phong
 * model of shading.
 */ 
class LightObject : public SceneObject {
private:
	vec4 diffuse;
	vec4 specular;
	vec4 ambient;
	float intensity;

public:
	LightObject(string objName, vec4 ambient,vec4 diffuse, vec4 specular, float intensity, vec3 position);

	// methods
	float getIntensity();
	void setIntensity(GLfloat newIntensity);

	vec4 getDiffuse();
	vec4 getSpecular();
	void setDiffuse(vec4 newDiff);
	void setSpecular(vec4 newSpec);

	virtual GLfloat getLightEffect(vec3 position);
	virtual void initlight(GLenum lightconstant);
	virtual LightType getType();

};

class PointLight : public LightObject {
public:	
	PointLight(string objName, vec4 ambient,vec4 diffuse, vec4 specular, float intensity, vec3 position);
};

class SpotLight : public LightObject {	
private: 
	vec3 direction;

public:
	SpotLight(string objName, vec4 ambient,vec4 diffuse, vec4 specular, float intensity, vec3 position, vec3 direction);


};

#endif
