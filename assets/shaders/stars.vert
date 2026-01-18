#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aBrightness;
layout (location = 2) in float aSize;

out float Brightness;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Remove translation from view matrix (keep rotation only)
    mat4 skyView = mat4(mat3(view));
    gl_Position = projection * skyView * vec4(aPos, 1.0);
    gl_PointSize = aSize;
    Brightness = aBrightness;
}
