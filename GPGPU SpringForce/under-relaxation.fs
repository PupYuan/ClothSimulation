#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D input_PosDelta1;
layout(rgba32f, binding = 1) uniform image2D output_PosData;//������Ŀ��ߵ����ͼ��


void main(void)
{
	//��computeShader�Ĳ�������ΪԼ��������
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	vec4 PosDelta1 = imageLoad(input_PosDelta1, pos);
	//�ۼ�����Լ��������λ�õ�ƫ��
	vec4 originPos = imageLoad(output_PosData, pos);
	originPos = originPos + PosDelta1;

	//�����������Ŀ��ߵ�������
	imageStore(output_PosData, pos, originPos);
}