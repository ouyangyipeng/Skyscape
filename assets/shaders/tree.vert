#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 VertexColor;
out vec3 FragPos;
out vec3 Normal;  // ✅ 添加法线输出

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // ✅ 修复：先计算世界坐标
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    
    // ✅ 简单法线计算
    if (aPos.y < 3.5) {
        // 树干：垂直向上的法线
        Normal = normalize(vec3(model * vec4(0.0, 1.0, 0.0, 0.0)));
    } else {
        // 树冠：从树冠中心向外的法线
        vec3 toCenter = normalize(aPos - vec3(0.0, 5.0, 0.0));
        Normal = normalize(vec3(model * vec4(toCenter, 0.0)));
    }
    
    VertexColor = aColor;
    
    // ✅ 关键修复：完整的 MVP 变换
    gl_Position = projection * view * worldPos;
}