attribute vec3 a_position;

varying vec4 v_color;

void main()
{
    v_color = vec4(1.0, 1.0, 1.0, 1.0);

    gl_Position = vec4(a_position, 1.0);
}
