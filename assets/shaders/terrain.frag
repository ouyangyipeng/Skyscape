#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

void main()
{
    float height = FragPos.y;
    vec3 baseColor;
    
    // ✅ 更平滑的颜色过渡
    if (height < -10.0) {
        baseColor = vec3(0.05, 0.15, 0.4);      // 深海
    } else if (height < -2.0) {
        float t = (height + 10.0) / 8.0;
        baseColor = mix(vec3(0.05, 0.15, 0.4), vec3(0.1, 0.4, 0.8), t);  // 深水->浅水
    } else if (height < 0.0) {
        baseColor = vec3(0.1, 0.4, 0.8);        // 浅水
    } else if (height < 1.0) {
        float t = height;
        baseColor = mix(vec3(0.1, 0.4, 0.8), vec3(0.9, 0.85, 0.6), t);  // 水->沙滩
    } else if (height < 3.0) {
        baseColor = vec3(0.9, 0.85, 0.6);       // 沙滩
    } else if (height < 15.0) {
        float t = (height - 3.0) / 12.0;
        baseColor = mix(vec3(0.9, 0.85, 0.6), vec3(0.3, 0.7, 0.2), t);  // 沙滩->草地
    } else if (height < 30.0) {
        baseColor = vec3(0.3, 0.7, 0.2);        // 草地
    } else if (height < 50.0) {
        float t = (height - 30.0) / 20.0;
        baseColor = mix(vec3(0.3, 0.7, 0.2), vec3(0.2, 0.45, 0.15), t);  // 草地->森林
    } else if (height < 70.0) {
        baseColor = vec3(0.2, 0.45, 0.15);      // 森林
    } else if (height < 90.0) {
        float t = (height - 70.0) / 20.0;
        baseColor = mix(vec3(0.2, 0.45, 0.15), vec3(0.5, 0.5, 0.5), t);  // 森林->岩石
    } else if (height < 110.0) {
        baseColor = vec3(0.5, 0.5, 0.5);        // 岩石
    } else {
        float t = min((height - 110.0) / 30.0, 1.0);
        baseColor = mix(vec3(0.5, 0.5, 0.5), vec3(0.98, 0.98, 1.0), t);  // 岩石->雪山
    }
    
    // ✅ 添加坡度影响（陡峭的地方显示岩石色）
    float slope = 1.0 - dot(normalize(Normal), vec3(0, 1, 0));
    if(slope > 0.6) {
        baseColor = mix(baseColor, vec3(0.4, 0.35, 0.3), (slope - 0.6) * 2.5);
    }
    
    // ✅ 改进的 Phong 光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // 环境光
    float ambientStrength = 0.35;
    vec3 ambient = ambientStrength * lightColor;
    
    // 漫反射（增强对比）
    float diff = max(dot(norm, lightDir), 0.0);
    diff = pow(diff, 0.8);  // ✅ 稍微增强漫反射
    vec3 diffuse = diff * lightColor;
    
    // 镜面反射（降低，让地形更自然）
    float specularStrength = 0.05;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;
    
    // ✅ 添加简单的雾效（远处淡化）
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-0.0008 * distance);  // 指数雾
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 fogColor = vec3(0.7, 0.8, 0.9);  // 天空色
    
    vec3 result = (ambient + diffuse + specular) * baseColor;
    result = mix(fogColor, result, fogFactor);  // 应用雾效
    
    FragColor = vec4(result, 1.0);
}