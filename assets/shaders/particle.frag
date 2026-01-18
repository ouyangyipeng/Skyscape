#version 330 core
in vec4 ParticleColor;
out vec4 FragColor;

void main()
{
    // Create circular particle shape
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    if (dist > 0.5)
        discard;
    
    // Soft edges
    float alpha = ParticleColor.a * (1.0 - smoothstep(0.3, 0.5, dist));
    
    FragColor = vec4(ParticleColor.rgb, alpha);
}
