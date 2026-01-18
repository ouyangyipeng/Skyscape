#version 330 core
in float Brightness;
out vec4 FragColor;

uniform float starVisibility;

void main()
{
    // Circular star shape
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    if (dist > 0.5)
        discard;
    
    // Soft glow
    float alpha = (1.0 - dist * 2.0) * Brightness * starVisibility;
    
    // Slightly yellow-white color
    vec3 starColor = vec3(1.0, 0.98, 0.9);
    
    FragColor = vec4(starColor, alpha);
}
