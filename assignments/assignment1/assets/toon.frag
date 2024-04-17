#version 330                          
out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;
in vec3 Color;

uniform vec3 uLightColor;

uniform float ambientK;
uniform float specularK;
uniform float rimK = 0.0;
uniform float rimThres;
uniform float rimCut;
//uniform float shininess;

uniform vec3 lightPos;
uniform vec3 eyePos;
uniform vec3 ObjColor;

void main(){
    vec3 _worldNorm = normalize(WorldNormal);
    float intensity = dot(lightPos, _worldNorm);
    vec3 color;

    //ambient
    vec3 ambient = ambientK * Color;
    vec3 toLight = normalize(lightPos - WorldPos);

    //specular
    vec3 toEye = normalize(eyePos - WorldPos);
    vec3 reflectDir = reflect(-toLight, _worldNorm);
    float s = pow(max(dot(toEye, reflectDir), 0), 32.0);
    vec3 specular = uLightColor * s *specularK;
    
    specular = smoothstep(0.005, 0.01, specular) * ObjColor; //makes the specular code not a gradient reflection, but more of a hard circle

    //rim shading
    toEye = normalize(eyePos - reflectDir); //Gets the distance between the eye position and the reflect direction
    float rim = dot(toEye, _worldNorm);
    rim = 1 - max(0.0, rim);
    float rimIntense = pow(rim, rimThres);
    rimIntense = smoothstep(rimCut - 0.01, rimCut + 0.01, rimIntense);//Makes the rimm appear in a toon shaded style


     float oof = smoothstep(0.0, 1.0, (intensity * 0.5) / rimK + 0.5); //Makes the ambient shadows, but makes it a hard line between lit and unlit
     color = mix(vec3(0.2) * ObjColor, uLightColor * ObjColor, oof);


    vec3 result = color * ObjColor * (ambient + specular + rimIntense);
    FragColor = vec4(result, 1.0f);
}
