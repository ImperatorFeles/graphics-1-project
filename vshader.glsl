#define MaxLights 8

struct LightSource {
	vec4  ambient;
	vec4  diffuse;	
	vec4  specular;
	vec3  position;
	vec4  halfVector;
	vec3  spotDirection;
	float spotExponent;
	float spotCutoff;
	
	float spotCosCutoff;
	float constantAttenuation;
	float linearAttenbuation;
	float quadraticAttenuation;
};
uniform LightSource Lights[MaxLights];

// Inputs from models buffer
attribute vec4 vPosition;
attribute vec3 vColor;
attribute vec3 vNormal;
attribute vec2 vTexture;

varying vec2 texcoord;
varying vec4 pcolor;

varying vec3 N;
varying vec3 L;
varying vec3 E;

uniform mat4 modeltransform;
uniform mat4 modelview;
uniform mat4 perspective;

uniform float shininess;

vec4 VertexLight() {
	int i = 0;
	
	vec3 pos = -(perspective * vPosition).xyz;

	vec3 L = normalize(Lights[i].position - pos);
	vec3 E = normalize(-pos);
	vec3 H = normalize(L + E);

	vec3 N = normalize(perspective * vec4(vNormal, 1.0)).xyz;

	vec4 ambient = Lights[i].ambient;

	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * Lights[i].diffuse;

	float Ks = pow(max(dot(N, H), 0.0), shininess);
	vec4 specular = Ks * Lights[i].specular;

	if (dot(L, N) < 0.0)
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	return ambient + specular + diffuse;
}

void main() {
	int i = 0;

	N = vNormal.xyz;
	L = Lights[i].position.xyz - -(perspective * vPosition).xyz;
	E = vPosition.xyz;

	//pcolor = VertexLight();
	//pcolor.a = 1.0;
	texcoord = vTexture;
   	gl_Position = perspective * modelview * modeltransform * vPosition;

	pcolor = vec4(vColor, 1.0);

}
