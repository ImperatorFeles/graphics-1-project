
varying  vec4 color;
varying float diffuse_value;

void
main()
{
    gl_FragColor = color * diffuse_value;
}
