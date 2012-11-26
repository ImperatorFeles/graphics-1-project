attribute  vec4 vPosition;
attribute  vec3 vColor;
attribute  vec3 vNormal;
varying vec4 color;

varying vec4 lightDiff;

uniform mat4 m;

void main()
{
	vec3 vertex_normal = normalize(gl_NormalMatrix * vNormal);
	vec3 vertex_light_position = gl_LightSource[0].position.xyz;
	lightDiff = gl_LightSource[0].diffuse * max(dot(vertex_normal, vertex_light_position), 0.0);

    	gl_Position = m * vPosition;
	color = vec4(vColor, 1);
}
