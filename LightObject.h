#ifndef _LIGHT_OBJECT
#define _LIGHT_OBJECT

#include "SceneObject.h"
#include "vec.h"

#include <vector>

using std::vector;

class LightObject : public SceneObject
{
 private:
  vec4 diffuse;
  vec4 specular;
  float intensity;

 public:
  LightObject(string objName, vec4 diffuse, vec4 specular, float intensity,
	      vec3 position);
  
  // methods
  float getIntensity();
  void setIntensity( float newIntensity );
  
  vec4 getDiff();
  vec4 getSpec();
  void setDiff( vec4 newDiff );
  void setSpec( vec4 newSpec );
  
};

#endif
