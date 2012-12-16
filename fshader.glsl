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

varying vec4 pcolor;
varying vec2 texcoord;

varying vec3 N;
varying vec3 L;
varying vec3 E;

uniform sampler2D gSampler;
uniform bool hasTexture;

uniform float shininess;

vec4 FragLight() {
	int i = 0;

	vec3 NN = normalize(N);
	vec3 EE = normalize(E);
	vec3 LL = normalize(L);
	
	vec4 ambient, diffuse, specular;
	vec3 H = normalize(LL+EE);
	float Kd = max(dot(LL,NN), 0.0);
	Kd = dot(LL, NN);
	float Ks = pow(max(dot(NN, H), 0.0), shininess);
	ambient = Lights[i].ambient;
	diffuse = Kd * Lights[i].diffuse;
	if (dot(LL, NN) < 0.0)
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	else
		specular = Ks * Lights[i].specular;
	return vec4((ambient + diffuse + specular).xyz, 1.0);
}

void main() {
	//gl_FragColor = pcolor;	
	//gl_FragColor = pcolor * texture2D(gSampler, texcoord);

	gl_FragColor = pcolor * FragLight() * texture2D(gSampler, texcoord);
}
