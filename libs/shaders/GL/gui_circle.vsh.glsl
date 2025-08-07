layout (location = 0) in vec2 a_position;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec3 a_outlineColor;
layout (location = 3) in float a_radius;
layout (location = 4) in float a_outlineWidthRatio;

layout (binding = 0) uniform UBO
{
  mat4 u_modelView;
  mat4 u_projection;
};

layout (location = 0) out vec2 v_position;
layout (location = 1) out vec3 v_color;
layout (location = 2) out vec3 v_outlineColor;
layout (location = 3) out float v_outlineWidthRatio;

void main()
{
    v_position = a_position;
    v_color = a_color;
    v_outlineColor = a_outlineColor;
    v_outlineWidthRatio = a_outlineWidthRatio;

    gl_Position = vec4(a_position * a_radius, 0, 1) * u_modelView * u_projection;
    #ifdef VULKAN
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z  + gl_Position.w) * 0.5;
    #endif
}