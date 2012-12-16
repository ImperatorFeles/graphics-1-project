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

struct MaterialProperties {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform MaterialProperties Material;

// Inputs from models buffer
attribute vec4 vPosition;
attribute vec3 vColor;
attribute vec3 vNormal;
attribute vec2 vTexture;

varying vec2 texcoord;
varying vec4 pcolor;

varying vec3 N;
varying vec3 E;
varying vec3 L[MaxLights];

uniform mat4 modeltransform;
uniform mat4 modelview;
uniform mat4 perspective;

uniform float shininess;

vec4 VertexLight() {
	int i = 0;
	vec4 total = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 pos, L, E, H, N;
	vec4 ambient, diffuse, specular;

	float Kd, Ks;
	
	for (i = 0; i < MaxLights; i++) {
		pos = -(perspective * vPosition).xyz;

		L = normalize(Lights[i].position - pos);
		E = normalize(-pos);
		H = normalize(L + E);

		N = normalize(perspective * vec4(vNormal, 1.0)).xyz;

		ambient = Lights[i].ambient;

		Kd = max(dot(L, N), 0.0);
		diffuse = Kd * Lights[i].diffuse;

		Ks = pow(max(dot(N, H), 0.0), shininess);
		specular = Ks * Lights[i].specular;

		if (dot(L, N) < 0.0)
			specular = vec4(0.0, 0.0, 0.0, 1.0);

		total += ambient + specular + diffuse;
	}
	return total;
}

void main() {
	int i = 0;

	N = vNormal.xyz;
	E = vPosition.xyz;

	for (i = 0; i < MaxLights; i++) 
		L[i] = Lights[i].position.xyz - -(perspective * vPosition).xyz;

	// Uncomment this to enable vertex shading
	//pcolor = VertexLight() * vec4(vColor, 1.0);
	//pcolor.a = 1.0;

	// Uncomment this to disable vertex shading
	pcolor = vec4(vColor, 1.0);

	texcoord = vTexture;
   	gl_Position = perspective * modelview * modeltransform * vPosition;
}
