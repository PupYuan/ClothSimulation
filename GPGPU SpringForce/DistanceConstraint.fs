#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D input_PosData;
layout(rg32i, binding = 1) uniform iimage2D input_IndexData1;//ÿ��Լ�����洢����
layout(rg32i, binding = 2) uniform iimage2D input_IndexData2;//Ӱ���������Ӿ�����������

layout(rgba32f, binding = 3) uniform image2D output_PosDeltaData;//������Ŀ��ߵ����ͼ��


void main(void)
{
	//��computeShader�Ĳ�������ΪԼ��������
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 index1 = ivec2(imageLoad(input_IndexData1, pos));
	ivec2 index2 = ivec2(imageLoad(input_IndexData2, pos));

	vec4 Pos1 = imageLoad(input_PosData, index1);
	vec4 Pos2 = imageLoad(input_PosData, index2);
	//Լ��������������������

	//�����������Ŀ��ߵ�������
	imageStore(output_PosDeltaData, index1, vec4(0,-0.01,0,0));
	imageStore(output_PosDeltaData, index2, vec4(0,-0.01,0,0));
}