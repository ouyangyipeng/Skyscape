#version 330 core
out vec4 FragColor;

in vec3 VertexColor;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main() {
    // ✅ 改进的光照模型
    vec3 lightColor = vec3(1.0, 0.95, 0.85);  // 温暖的阳光
    
    // 环境光（提高基础亮度）
    float ambientStrength = 0.5;  // ✅ 从 0.4 提高到 0.5
    vec3 ambient = ambientStrength * lightColor;
    
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.8;  // ✅ 增强漫反射
    
    // 高光（树叶有轻微反光）
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    vec3 specular = 0.15 * spec * lightColor;  // ✅ 从 0.1 提高到 0.15
    
    // 背光效果（半透明树叶）
    float backlight = max(0.0, -dot(norm, lightDir)) * 0.4;  // ✅ 从 0.3 提高到 0.4
    
    // ✅ 添加随机噪声增加细节
    float noise = fract(sin(dot(FragPos.xz, vec2(12.9898, 78.233))) * 43758.5453);
    vec3 detailColor = VertexColor * (0.85 + noise * 0.3);  // ✅ 增加噪声影响
    
    // ✅ 防止过暗：确保最小亮度
    vec3 result = (ambient + diffuse + specular + backlight) * detailColor;
    result = max(result, VertexColor * 0.3);  // ✅ 保证最低 30% 的原色
    
    // ✅ 距离雾效（减弱雾效影响）
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-0.00005 * distance * distance);  // ✅ 从 0.0001 降低到 0.00005
    fogFactor = clamp(fogFactor, 0.5, 1.0);  // ✅ 限制雾效不能太浓
    vec3 fogColor = vec3(0.7, 0.8, 0.9);
    result = mix(fogColor, result, fogFactor);
    
    FragColor = vec4(result, 1.0);
}