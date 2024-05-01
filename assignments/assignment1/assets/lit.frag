#version 450
layout(location = 0) out vec4 FragColor; //The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space
	vec2 TexCoord;
	mat3 TBN;
	vec2 UV;
}fs_in;

uniform sampler2D _MainTex; 
uniform sampler2D normalMap;
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightPos = vec3(-1.0, -1.0, 0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
uniform sampler2D _ColorBuffer;

uniform float rimThres;
uniform float rimCut;

struct Material{
	float Ka; //Ambient coefficient (0-1)
	float Kd; //Diffuse coefficient (0-1)
	float Ks; //Specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;

void main(){
	//Make sure fragment normal is still length 1 after interpolation.
	vec3 normal = normalize(fs_in.WorldNormal);

	normal = texture(normalMap, fs_in.TexCoord).rgb;
	normal = normal * 2.0 - 1.0;   
	normal = normalize(fs_in.TBN * normal); 
	
	float intensity = dot(_LightPos, normal);

	//Light pointing straight down
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	vec3 downLight = -_LightDirection;
	float diffuseFactor = max(dot(normal,downLight),0.0);
	//Calculate specularly reflected light
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 toLight = normalize(_LightPos - fs_in.WorldPos);
	//Blinn-phong uses half angle
	vec3 h = normalize(downLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	vec3 specular = smoothstep(0.005, 0.01, specularFactor) * objectColor;
	//Combination of specular and diffuse reflection
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specular) * _LightColor;
	lightColor += _AmbientColor * _Material.Ka;


	// Toon Shading
	float rimLight = 1.0 - max(dot(normalize(_EyePos - fs_in.WorldPos), normal), 0.0); // Calculate rim lighting based on angel between EyePos and nornmal
    float rimIntensity = pow(rimLight, rimThres);
    rimIntensity = smoothstep(rimCut - 0.01, rimCut + 0.01, rimIntensity); //Makes the rimmLight appear in a toon shaded style
	lightColor += rimIntensity;

	float rimShadow = smoothstep(0.0, 1.0, (intensity * 0.5) / rimCut + 0.5); //Makes the ambient shadows, but makes it a hard line between lit and unlit
    lightColor = mix(vec3(0.2) * objectColor, lightColor * objectColor, rimShadow);

	FragColor = vec4(objectColor * lightColor, 1.0);
}
