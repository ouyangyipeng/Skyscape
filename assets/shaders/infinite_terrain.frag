#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main() {
    // Ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular (subtle)
    float specularStrength = 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Distance fog
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp(1.0 - (distance - 500.0) / 1500.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.7, 0.8, 0.9); // Light blue fog
    
    vec3 result = (ambient + diffuse + specular) * VertexColor;
    result = mix(fogColor, result, fogFactor);
    
    FragColor = vec4(result, 1.0);
}
