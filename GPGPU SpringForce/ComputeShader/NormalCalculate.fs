#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D output_PosData;//������Ŀ��ߵ����ͼ��
layout(r32i, binding = 1) uniform iimage2D NormalX;
layout(r32i, binding = 2) uniform iimage2D NormalY;
layout(r32i, binding = 3) uniform iimage2D NormalZ;

void main(void)
{
	//��computeShader�Ĳ�������ΪԼ��������
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	//���ߵļ���
	vec3 normal = vec3(0);

	//��һ�������εķ���
	vec4 p1 = imageLoad(output_PosData, ivec2(pos.x+1,pos.y));
	vec4 p2 = imageLoad(output_PosData, ivec2(pos.x,pos.y));
	vec4 p3 = imageLoad(output_PosData, ivec2(pos.x,pos.y+1));
	vec4 v1 = p2-p1;
	vec4 v2 = p3-p1;
	normal = cross(v1.xyz,v2.xyz);
	imageAtomicAdd(NormalX,ivec2(pos.x+1,pos.y),int(normal.x*1000.0f));
	imageAtomicAdd(NormalY,ivec2(pos.x+1,pos.y),int(normal.y*1000.0f));
	imageAtomicAdd(NormalZ,ivec2(pos.x+1,pos.y),int(normal.z*1000.0f));

	imageAtomicAdd(NormalX,ivec2(pos.x,pos.y),int(normal.x*1000.0f));
	imageAtomicAdd(NormalY,ivec2(pos.x,pos.y),int(normal.y*1000.0f));
	imageAtomicAdd(NormalZ,ivec2(pos.x,pos.y),int(normal.z*1000.0f));

	imageAtomicAdd(NormalX,ivec2(pos.x,pos.y+1),int(normal.x*1000.0f));
	imageAtomicAdd(NormalY,ivec2(pos.x,pos.y+1),int(normal.y*1000.0f));
	imageAtomicAdd(NormalZ,ivec2(pos.x,pos.y+1),int(normal.z*1000.0f));
     //�ڶ��������εķ���
	p1 = imageLoad(output_PosData, ivec2(pos.x+1,pos.y+1));
	p2 = imageLoad(output_PosData, ivec2(pos.x+1,pos.y));
	p3 = imageLoad(output_PosData, ivec2(pos.x,pos.y+1));
	v1 = p2-p1;
	v2 = p3-p1;
	normal = cross(v1.xyz,v2.xyz);
	imageAtomicAdd(NormalX,ivec2(pos.x+1,pos.y+1),int(normal.x*1000.0f));
	imageAtomicAdd(NormalY,ivec2(pos.x+1,pos.y+1),int(normal.y*1000.0f));
	imageAtomicAdd(NormalZ,ivec2(pos.x+1,pos.y+1),int(normal.z*1000.0f));

	imageAtomicAdd(NormalX,ivec2(pos.x+1,pos.y),int(normal.x*1000.0f));
	imageAtomicAdd(NormalY,ivec2(pos.x+1,pos.y),int(normal.y*1000.0f));
	imageAtomicAdd(NormalZ,ivec2(pos.x+1,pos.y),int(normal.z*1000.0f));

	imageAtomicAdd(NormalX,ivec2(pos.x,pos.y+1),int(normal.x*1000.0f));
	imageAtomicAdd(NormalY,ivec2(pos.x,pos.y+1),int(normal.y*1000.0f));
	imageAtomicAdd(NormalZ,ivec2(pos.x,pos.y+1),int(normal.z*1000.0f));
}