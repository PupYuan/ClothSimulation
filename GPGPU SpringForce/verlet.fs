#version 330 core
layout (location = 0) out vec4 gPosition;

in vec2 TexCoords;

uniform sampler2D X;					//current position
uniform sampler2D X_last;			//previous position

void main() {
	vec3 x_i = texture2D(X, TexCoords).xyz;
	vec3 x_last = texture2D(X_last, TexCoords).xyz;
    x_i.y = x_i.y - 0.01;
	// fragment outputs
	gPosition = vec4(x_i,1.0);
}