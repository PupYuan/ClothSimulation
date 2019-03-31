#version 330 core
layout (location = 0) out vec4 gPosition;

in vec2 TexCoords;

uniform sampler2D X;					//current position

void main() {
	vec3 x_i = texture2D(X, TexCoords).xyz;
		
	// fragment outputs
	gPosition = vec4(x_i,1.0);
}