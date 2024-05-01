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
    mat3 sob;

    // gets each point of the image 
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 sampler = texelFetch(normalMap, ivec2(gl_FragCoord) + ivec2(i - offset,j - offset), 0).rgb * gasblur[j][i];
            sob[i][j] = length(sampler); 
        }
    }

    // applies kernels to image
    float gx = dot(edgeX[0], sob[0]) + dot(edgeX[1], sob[1]) + dot(edgeX[2], sob[2]); 
    float gy = dot(edgeY[0], sob[0]) + dot(edgeY[1], sob[1]) + dot(edgeY[2], sob[2]);
    float sobel = sqrt(pow(gx, 2.0) + pow(gy, 2.0));

    // thickness of outline
    sobel = smoothstep(outlineX, outlineY, sobel);

    FragColor = vec4(mix(diff, EdgeColor, sobel), 1.0);
} 