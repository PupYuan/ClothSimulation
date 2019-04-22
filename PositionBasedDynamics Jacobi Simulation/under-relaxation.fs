#version 330 core
layout (location = 0) out vec4 PositionDelta1;
layout (location = 1) out vec2 Index1;
layout (location = 2) out vec4 PositionDelta2;
layout (location = 3) out vec2 Index2;

in vec2 TexCoords;

uniform sampler2D X;					//current position
uniform sampler2D Constraints;          //约束纹理



void main() {
    //从索引中获得的粒子索引
	vec2 C_1 = texture2D(Constraints, TexCoords).xy;
	vec2 C_2 = texture2D(Constraints, TexCoords).zw;

	vec3 x_1 = texture2D(X,C_1).xyz;
	vec3 x_2 = texture2D(X,C_2).xyz;

	//经历了约束之后得到


	//输出
	PositionDelta1 = vec4(x_1,1.0);
	Index1 = vec4(C_1,0.0);
	PositionDelta2 = vec4(x_2,1.0);
	Index2 = vec4(C_2,0.0);
}