#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float iTime;
uniform sampler2D snowTex;
uniform sampler2D rockTex;
uniform sampler2D waterTex;

void main() {
    vec3 norm = normalize(Normal);
    
    // Enhanced ambient lighting
    float skyLight = 0.5 * (0.6 + 0.4 * norm.y);
    float groundLight = 0.2 * (0.6 - 0.4 * norm.y);
    vec3 ambient = skyLight * vec3(0.7, 0.8, 1.0) + groundLight * vec3(0.4, 0.35, 0.3);
    ambient *= 0.7;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.12;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 24);
    vec3 specular = specularStrength * spec * lightColor;

    // Distance fog
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp(1.0 - (distance - 500.0) / 1500.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.7, 0.8, 0.9);

    // Texture sampling
    vec2 uv = FragPos.xz * 0.02;
    vec3 rockTexColor = texture(rockTex, uv).rgb;
    
    // Use ONLY rock texture for everything
    vec3 baseColor = rockTexColor;

    // Final color
    vec3 result = (ambient + diffuse) * baseColor + specular;
    result = mix(fogColor, result, fogFactor);

    FragColor = vec4(result, 1.0);
}

