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

/* TODO: For future.
struct MaterialProperties {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform MaterialProperties Material;
*/

varying vec4 pcolor;
varying vec2 texcoord;

varying vec3 N;
varying vec3 E;
varying vec3 L[MaxLights];

uniform sampler2D gSampler;
uniform bool hasTexture;

uniform float shininess;

vec4 FragLight() {
	int i = 0;
	vec4 total = vec4(0.0, 0.0, 0.0, 0.0);

	vec3 NN, LL, EE, H;
	vec4 ambient, diffuse, specular;

	float Kd, Ks;

	for (i = 0; i < MaxLights; i++) {
		NN = normalize(N);
		EE = normalize(E);
		LL = normalize(L[i]);
		H = normalize(LL+EE);

		Kd = max(dot(LL,NN), 0.0);
		Ks = pow(max(dot(NN, H), 0.0), shininess);

		ambient = Lights[i].ambient;
		diffuse = Kd * Lights[i].diffuse;
		if (dot(LL, NN) < 0.0)
			specular = vec4(0.0, 0.0, 0.0, 1.0);
		else
			specular = Ks * Lights[i].specular;

		total += specular + diffuse + ambient;
	}
	return vec4(total.xyz, 1.0);
}

void main() {
	// Uncomment this for no fragment lighting or texture
	//gl_FragColor = pcolor;	

	// Uncomment this for no fragment lighting
	//gl_FragColor = pcolor * texture2D(gSampler, texcoord);

	// Uncomment this for fragment lighting
	gl_FragColor = pcolor * FragLight() * texture2D(gSampler, texcoord);
}
