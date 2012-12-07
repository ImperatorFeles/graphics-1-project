varying vec4 color;
varying vec4 lightDiff;
varying vec2 outtexture;

uniform sampler2D gSampler;

void
main()
{
    gl_FragColor = texture2D(gSampler, outtexture) * lightDiff;
}
