attribute  vec4 vPosition;
attribute  vec3 vColor;
varying vec4 color;

uniform mat4 m;

void main()
{
    	gl_Position = m * vPosition;
	color = vec4(vColor, 1);
}
