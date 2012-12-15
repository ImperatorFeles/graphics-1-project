varying vec4 pcolor;
varying vec2 texcoord;

uniform sampler2D gSampler;
uniform bool hasTexture;

void main() {
	//gl_FragColor = pcolor;	
	gl_FragColor = pcolor * texture2D(gSampler, texcoord);
}
