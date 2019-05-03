#version 330 core
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
	Normal = mat3(transpose(inverse(model))) * vec3(Normalx/100000.0f,Normaly/100000.0f,Normalz/100000.0f);  
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}