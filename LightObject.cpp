#include "LightObject.h"
#include "vec.h"

#include <iostream>

using std::cout;
using std::vector;

LightObject::LightObject(string objName, vec4 diffuse, vec4 specular, float intensity, vec3 position):
  SceneObject(objName, position), diffuse(diffuse), specular(specular), intensity(intensity)
{
}

float LightObject::getIntensity()
{
  return this->intensity;
}
void LightObject::setIntensity( float newIntensity )
{
  this->intensity = newIntensity;
}

vec4 LightObject::getDiff()
{
  return this->diffuse;
}
void LightObject::setDiff( vec4 newDiff )
{
  this->diffuse = newDiff;
}

vec4 LightObject::getSpec()
{
  return this->specular;
}
void LightObject::setSpec( vec4 newSpec)
{
  this->specular= newSpec;
}
