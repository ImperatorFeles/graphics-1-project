attribute vec4 vPosition;
attribute vec3 vColor;
attribute vec3 vNormal;
attribute vec2 vTexture;

varying vec4 color;

varying vec4 lightDiff;

varying vec2 outtexture;

uniform mat4 m;

void main()
{
	vec3 vertex_normal = normalize(gl_NormalMatrix * vNormal);
	vec3 vertex_light_pos0 = gl_LightSource[0].position.xyz;
	vec3 vertex_light_pos1 = gl_LightSource[1].position.xyz;

	vec4 light0Diff = gl_LightSource[0].diffuse * max(dot(vertex_normal, vertex_light_pos0), 0.0);
	vec4 light1Diff = gl_LightSource[1].diffuse * max(dot(vertex_normal, vertex_light_pos1), 0.0);
	lightDiff = light0Diff + light1Diff;

	outtexture = vTexture;

   	gl_Position = m * vPosition;
	color = vec4(vColor, 1);
}
