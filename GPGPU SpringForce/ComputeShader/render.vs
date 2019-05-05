#version 440 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in int Normalx;
layout (location = 2) in int Normaly;
layout (location = 3) in int Normalz;
layout (location = 4) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * aPos);
	float normalx = float(Normalx/1000.0f);
	float normlay = float(Normaly/1000.0f);
	float normalz = float(Normalz/1000.0f);
	Normal = mat3(transpose(inverse(model))) * vec3(normalx,normlay,normalz);  
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}