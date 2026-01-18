#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 VertexColor;
out vec3 FragPos;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 flowerColor; // ✅ 实例化颜色

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Height = aPos.y;
    
    // ✅ 将基础颜色与花朵颜色相乘
    VertexColor = aColor * flowerColor;
    
    gl_Position = projection * view * worldPos;
}