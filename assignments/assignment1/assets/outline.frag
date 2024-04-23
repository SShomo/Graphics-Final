#version 450
layout (location = 0) out vec4 FragColor;
in vec2 UV;

mat3 edgeX = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 edgeY = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

vec3 gasblur[3] = {
	vec3( 1, 2, 1),
	vec3( 2, 4, 2), 
	vec3( 1, 2, 1) 
};

uniform sampler2D normalMap;
uniform vec3 EdgeColor;
uniform float outlineX;
uniform float outlineY;

uniform float offset;


void main()
{
    vec3 diff = texture(normalMap, UV.st).rgb;
    mat3 I;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 sampler = texelFetch(normalMap, ivec2(gl_FragCoord) + ivec2(i - offset,j - offset), 0).rgb * gasblur[j][i];
            I[i][j] = length(sampler); 
        }
    }

    float gx = dot(edgeX[0], I[0]) + dot(edgeX[1], I[1]) + dot(edgeX[2], I[2]); 
    float gy = dot(edgeY[0], I[0]) + dot(edgeY[1], I[1]) + dot(edgeY[2], I[2]);

    float g = sqrt(pow(gx, 2.0) + pow(gy, 2.0));

    // Try different values and see what happens
    g = smoothstep(outlineX, outlineY, g);


    FragColor = vec4(mix(diff, EdgeColor, g), 1.0);
} 