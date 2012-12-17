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

void LightObject::setLightIndex(GLint index) {
	info.lightIndex = index;	
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
	if (info.enabled) {	
		glUniform4fv(info.ids.ambient_id, 1, info.values.ambient);
		glUniform4fv(info.ids.diffuse_id, 1, info.values.diffuse);
		glUniform4fv(info.ids.specular_id, 1, info.values.specular);
		glUniform3fv(info.ids.position_id, 1, SceneObject::position);

		// TODO: Add more bindings
	}
}

void LightObject::translate(vec3 translation) {
	SceneObject::translate(translation);

	/*
	position.x += translation.x;
	position.y += translation.y;
	position.z += translation.z;
	*/
}

void LightObject::disable() { 
	info.enabled = false;
}

void LightObject::enable() {
	info.enabled = true;
}


