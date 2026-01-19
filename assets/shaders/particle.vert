#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aSize;

out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    
    // ✅ 基于距离动态调整尺寸（远处粒子更小）
    float distance = length((view * vec4(aPos, 1.0)).xyz);
    float scaleFactor = 1000.0 / max(distance, 1.0); // ✅ 增大系数
    gl_PointSize = aSize * scaleFactor;
    gl_PointSize = clamp(gl_PointSize, 2.0, 50.0); // ✅ 限制范围 2-50 像素

    ParticleColor = aColor;
}
