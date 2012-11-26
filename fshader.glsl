
varying vec4 color;
varying vec4 lightDiff;

void
main()
{
    gl_FragColor = color * lightDiff;
}
