#include "LightObject.h"

#include "vec.h"

LightObject::LightObject() :
	SceneObject("") {
	
	memset(&info, 0, sizeof(info));
}

LightObject::LightObject(string objName, vec3 position, LightInfo info) :
	SceneObject(objName, position),
	info(info) {

	// Nothing			
}

void LightObject::bindLight(GLint program) {
	char str[80];
	
	// Basic Properties
	snprintf(str, 80, "Lights[%d].ambient", info.lightIndex);
	info.ids.ambient_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].diffuse", info.lightIndex);
	info.ids.diffuse_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].specular", info.lightIndex);
	info.ids.specular_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].position", info.lightIndex);
	info.ids.position_id = glGetUniformLocation(program, str);

	if (!info.isSpotLight) return;

	snprintf(str, 80, "Lights[%d].spotDirection", info.lightIndex);
	info.ids.spotDirection_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].spotExponent", info.lightIndex);
	info.ids.spotDirection_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].constantAttenuation", info.lightIndex);
	info.ids.constantAttenuation_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].linearAttenbuation", info.lightIndex);
	info.ids.linearAttenbuation_id = glGetUniformLocation(program, str);
	snprintf(str, 80, "Lights[%d].quadraticAttenuation", info.lightIndex);
	info.ids.quadraticAttenuation_id = glGetUniformLocation(program, str);
}

void LightObject::setValues() {
	glUniform1fv(info.ids.ambient_id, 1, info.values.ambient);
	glUniform1fv(info.ids.diffuse_id, 1, info.values.diffuse);
	glUniform1fv(info.ids.specular_id, 1, info.values.specular);
	glUniform1fv(info.ids.position_id, 1, SceneObject::position);

	// TODO: Add more bindings
}

