#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;//先固定为1482个约束，后面扩展成动态的

uniform float wi;//粒子质量倒数，假定所有粒子质量保持一致，若不一致则需要用一张纹理存储
uniform float k_prime;
uniform float global_dampening;	//default velocity damping

layout(rgba32f, binding = 0) uniform image2D input_PosData;
layout(rg32i, binding = 1) uniform iimage2D input_IndexData1;//每个约束都存储索引
layout(rg32i, binding = 2) uniform iimage2D input_IndexData2;
layout(rg32i, binding = 3) uniform iimage2D input_IndexData3;

layout(r32i, binding = 4) uniform iimage2D output_PosDeltaDataX;//粒子数目宽高的输出图像
layout(r32i, binding = 5) uniform iimage2D output_PosDeltaDataY;//粒子数目宽高的输出图像
layout(r32i, binding = 6) uniform iimage2D output_PosDeltaDataZ;//粒子数目宽高的输出图像
layout(r32f, binding = 7) uniform image2D input_restDistance;//粒子之间的距离


void main(void)
{
	//该computeShader的并行数量为约束的数量
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 index1 = ivec2(imageLoad(input_IndexData1, pos));
	ivec2 index2 = ivec2(imageLoad(input_IndexData2, pos));
	ivec2 index3 = ivec2(imageLoad(input_IndexData3, pos));

	vec4 Pos1 = imageLoad(input_PosData, index1);
	vec4 Pos2 = imageLoad(input_PosData, index2);
	vec4 Pos3 = imageLoad(input_PosData, index3);
	float global_k = global_dampening * 0.01f;
	vec3 center = 0.3333f * (Pos1.xyz + Pos2.xyz + Pos3.xyz);
	vec3 dir_center = vec3(Pos3) - center;
	float dist_center = length(dir_center);
	//约束对这两个粒子起作用
	float rest_length = float(imageLoad(input_restDistance, pos));
	float diff = 1.0f - ((global_k + rest_length) / dist_center);
	vec3 dir_force = dir_center * diff;
	vec3 fa = k_prime * ((2.0f) / 4.0f) * dir_force;
	vec3 fb =k_prime * ((2.0f) /4.0f) * dir_force;
	vec3 fc =(-1.0f)*k_prime * ((4.0f) /4.0f) * dir_force;

	imageAtomicAdd(output_PosDeltaDataX,index1.xy,int(fa.x*1000));
	imageAtomicAdd(output_PosDeltaDataY,index1.xy,int(fa.y*1000));
	imageAtomicAdd(output_PosDeltaDataZ,index1.xy,int(fa.z*1000));

	imageAtomicAdd(output_PosDeltaDataX,index2.xy,int(fb.x*1000));
	imageAtomicAdd(output_PosDeltaDataY,index2.xy,int(fb.y*1000));
	imageAtomicAdd(output_PosDeltaDataZ,index2.xy,int(fb.z*1000));

	imageAtomicAdd(output_PosDeltaDataX,index3.xy,int(fc.x*1000));
	imageAtomicAdd(output_PosDeltaDataY,index3.xy,int(fc.y*1000));
	imageAtomicAdd(output_PosDeltaDataZ,index3.xy,int(fc.z*1000));
}