#version 330 core
out vec4 FragColor;

in vec3 VertexColor;
in vec3 FragPos;
in float Height;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main() {
    vec3 lightColor = vec3(1.0, 0.95, 0.85);
    
    // 环境光
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;
    
    // 简单漫反射（假设花朵法线向上）
    vec3 norm = normalize(vec3(0.0, 1.0, 0.2)); // 略微向前倾斜
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.6;
    
    // ✅ 根据高度增加亮度变化（花瓣顶部更亮）
    float heightFactor = Height * 0.3 + 0.7;
    
    // ✅ 背光效果（半透明花瓣）
    float backlight = max(0.0, -dot(norm, lightDir)) * 0.5;
    
    vec3 result = (ambient + diffuse + backlight) * VertexColor * heightFactor;
    
    // ✅ 距离雾效
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-0.00003 * distance * distance);
    fogFactor = clamp(fogFactor, 0.3, 1.0);
    vec3 fogColor = vec3(0.7, 0.8, 0.9);
    result = mix(fogColor, result, fogFactor);
    
    // ✅ 增加饱和度（让花朵颜色更鲜艳）
    result = result * 1.2;
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}