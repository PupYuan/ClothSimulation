#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;//先固定为1482个约束，后面扩展成动态的

uniform float wi;//粒子质量倒数，假定所有粒子质量保持一致，若不一致则需要用一张纹理存储
uniform float k_prime;

layout(rgba32f, binding = 0) uniform image2D input_PosData;
layout(rg32i, binding = 1) uniform iimage2D input_IndexData1;//每个约束都存储索引
layout(rg32i, binding = 2) uniform iimage2D input_IndexData2;//影响两个粒子就用两个纹理

layout(r32i, binding = 3) uniform iimage2D output_PosDeltaDataX;//粒子数目宽高的输出图像
layout(r32i, binding = 4) uniform iimage2D output_PosDeltaDataY;//粒子数目宽高的输出图像
layout(r32i, binding = 5) uniform iimage2D output_PosDeltaDataZ;//粒子数目宽高的输出图像
layout(r32f, binding = 6) uniform image2D input_restDistance;//粒子之间的距离


void main(void)
{
	//该computeShader的并行数量为约束的数量
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 index1 = ivec2(imageLoad(input_IndexData1, pos));
	ivec2 index2 = ivec2(imageLoad(input_IndexData2, pos));

	vec4 Pos1 = imageLoad(input_PosData, index1);
	vec4 Pos2 = imageLoad(input_PosData, index2);
	//约束对这两个粒子起作用
	float d = float(imageLoad(input_restDistance, pos));
	float distanceP1P2  = distance(Pos1,Pos2) - d;

	vec4 deltaP1 = (-wi) / (wi + wi) * (distanceP1P2)*(Pos1 - Pos2) / (distance(Pos1, Pos2))*k_prime;
	vec4 deltaP2 = (wi) / (wi + wi) * (distanceP1P2)*(Pos1 - Pos2) / (distance(Pos1, Pos2))*k_prime;

	imageAtomicAdd(output_PosDeltaDataX,index1.xy,int(deltaP1.x*1000));
	imageAtomicAdd(output_PosDeltaDataY,index1.xy,int(deltaP1.y*1000));
	imageAtomicAdd(output_PosDeltaDataZ,index1.xy,int(deltaP1.z*1000));

	imageAtomicAdd(output_PosDeltaDataX,index2.xy,int(deltaP2.x*1000));
	imageAtomicAdd(output_PosDeltaDataY,index2.xy,int(deltaP2.y*1000));
	imageAtomicAdd(output_PosDeltaDataZ,index2.xy,int(deltaP2.z*1000));
}