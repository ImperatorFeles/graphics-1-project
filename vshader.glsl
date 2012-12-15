struct Light {
	vec4 diff;
	vec4 ambi;
	vec4 spec;
	vec4 position;
};

// Inputs from models buffer
attribute vec4 vPosition;
attribute vec3 vColor;
attribute vec3 vNormal;
attribute vec2 vTexture;

varying vec2 texcoord;
varying vec4 pcolor;

uniform mat4 modeltransform;
uniform mat4 modelview;
uniform mat4 perspective;

uniform float shininess;
uniform Light inLight;

void main() {
	vec3 pos = -(modelview * vPosition).xyz;

	vec3 L = normalize(inLight.position.xyz - pos);
	vec3 E = normalize(-pos);
	vec3 H = normalize(L + E);

	vec3 N = normalize(modelview * vec4(vNormal, 1.0)).xyz;

	vec4 ambient = inLight.ambi;

	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * inLight.diff;

	float Ks = pow(max(dot(N, H), 0.0), shininess);
	vec4 specular = Ks * inLight.spec;

	if (dot(L, N) < 0.0)
		specular = vec4(0.0, 0.0, 0.0, 1.0);

	texcoord = vTexture;

   	gl_Position = perspective * modelview * modeltransform * vPosition;
	
	pcolor = ambient + specular + diffuse;
	pcolor.a = 1.0;
}
