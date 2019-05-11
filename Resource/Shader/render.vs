#version 440 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in int Normalx;
layout (location = 2) in int Normaly;
layout (location = 3) in int Normalz;
layout (location = 4) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main()
{
    FragPos = vec3(model * aPos);
	float normalx = float(Normalx/10000.0f);
	float normlay = float(Normaly/10000.0f);
	float normalz = float(Normalz/10000.0f);
	Normal = mat3(transpose(inverse(model))) * vec3(normalx,normlay,normalz);  
    TexCoords = aTexCoords;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}