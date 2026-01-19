#version 330 core
in vec4 ParticleColor;
out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    // ✅ 根据透明度判断粒子类型
    if (ParticleColor.a >= 0.9) { 
        // 雨滴：垂直拉伸（椭圆形）
        coord.x *= 8.0; // ✅ 拉伸为细长条纹
        dist = length(coord);

        
        if (dist > 0.5)
            discard;

        
        // ✅ 雨滴边缘锐利
        float alpha = ParticleColor.a * (1.0 - smoothstep(0.35, 0.5, dist));
        FragColor = vec4(ParticleColor.rgb, alpha);
    } 
    else { 
        // 雪花：圆形，边缘柔和
        if (dist > 0.5)
            discard;
        
        float alpha = ParticleColor.a * (1.0 - smoothstep(0.2, 0.5, dist));
        
        // 添加亮度渐变
        float brightness = 1.0 - dist * 0.3;
        vec3 color = ParticleColor.rgb * brightness;
        
        FragColor = vec4(color, alpha);
    }
}