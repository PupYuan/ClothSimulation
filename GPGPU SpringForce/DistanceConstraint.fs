#version 430 core

layout(local_size_x = 1482, local_size_y = 1, local_size_z = 1) in;//先固定为1024个，后面扩展成动态的

uniform float restDistance;	//假定所有粒子之间的距离保持一致，若不一致则需要用一张纹理存储
uniform float wi;//粒子质量倒数，假定所有粒子质量保持一致，若不一致则需要用一张纹理存储
uniform float k_prime;

layout(rgba32f, binding = 0) uniform image2D input_PosData;
layout(rg32i, binding = 1) uniform iimage2D input_IndexData1;//每个约束都存储索引
layout(rg32i, binding = 2) uniform iimage2D input_IndexData2;//影响两个粒子就用两个纹理

layout(rgba32f, binding = 3) uniform image2D output_PosDeltaData;//粒子数目宽高的输出图像
layout(r32f, binding = 4) uniform image2D input_restDistance;//粒子之间的距离

shared ivec4 particleData[30][30];//粒子数据

void main(void)
{
	//该computeShader的并行数量为约束的数量
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 index1 = ivec2(imageLoad(input_IndexData1, pos));
	ivec2 index2 = ivec2(imageLoad(input_IndexData2, pos));

	vec4 Pos1 = imageLoad(input_PosData, index1);
	vec4 Pos2 = imageLoad(input_PosData, index2);
	particleData[index1.x][index1.y] = ivec4(0,0,0,0);
	particleData[index2.x][index2.y] = ivec4(0,0,0,0);
	//同步数据
	barrier();
	//约束对这两个粒子起作用
	//float d = restDistance;
	float d = float(imageLoad(input_restDistance, pos));
	float distanceP1P2  = distance(Pos1,Pos2) - d;

	vec4 deltaP1 = (-wi) / (wi + wi) * (distanceP1P2)*(Pos1 - Pos2) / (distance(Pos1, Pos2))*k_prime;
	vec4 deltaP2 = (wi) / (wi + wi) * (distanceP1P2)*(Pos1 - Pos2) / (distance(Pos1, Pos2))*k_prime;

	ivec4 deltaP1Int = ivec4(deltaP1*10000);
	ivec4 deltaP2Int = ivec4(deltaP2*10000);
	atomicAdd(particleData[index1.x][index1.y].x, deltaP1Int.x);
	atomicAdd(particleData[index1.x][index1.y].y, deltaP1Int.y);
	atomicAdd(particleData[index1.x][index1.y].z, deltaP1Int.z);
	atomicAdd(particleData[index1.x][index1.y].w, deltaP1Int.w);

	atomicAdd(particleData[index2.x][index2.y].x, deltaP2Int.x);
	atomicAdd(particleData[index2.x][index2.y].y, deltaP2Int.y);
	atomicAdd(particleData[index2.x][index2.y].z, deltaP2Int.z);
	atomicAdd(particleData[index2.x][index2.y].w, deltaP2Int.w);
	//particleData里面的数据全部存储完成后再进行累加
	barrier();
	vec4 data1 = particleData[index1.x][index1.y]/10000.0f;
	vec4 data2 = particleData[index2.x][index2.y]/10000.0f;
	//这里会出现并行问题，两个约束同时运行，共同影响同一个粒子（x,y），此时在另一个工作组中，imageStore到output_PosDeltaData的同一个位置
	//相同种类的约束会出现冲突
	imageStore(output_PosDeltaData, index1.xy, data1);
	imageStore(output_PosDeltaData, index2.xy, data2);
	
}