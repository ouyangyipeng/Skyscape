#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main() {
    // Multi-color fighter jet based on local position
    vec3 objectColor;
    
    // Cockpit area (top front) - dark blue glass
    if (LocalPos.y > 0.3 && LocalPos.z > 1.0) {
        objectColor = vec3(0.1, 0.15, 0.3);
    }
    // Wings - dark gray
    else if (abs(LocalPos.x) > 0.5) {
        objectColor = vec3(0.3, 0.32, 0.35);
    }
    // Tail area - darker
    else if (LocalPos.z < -1.0) {
        objectColor = vec3(0.25, 0.25, 0.28);
    }
    // Engine exhaust
    else if (LocalPos.z < -1.8) {
        objectColor = vec3(0.15, 0.15, 0.18);
    }
    // Main body - military gray
    else {
        objectColor = vec3(0.45, 0.47, 0.5);
    }
    
    // Add some panel line effect
    float panelNoise = sin(LocalPos.x * 20.0) * sin(LocalPos.z * 15.0) * 0.02;
    objectColor += vec3(panelNoise);
    
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular (metallic look)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
