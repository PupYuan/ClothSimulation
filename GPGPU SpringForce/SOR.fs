#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D input_PosDelta1;
layout(rgba32f, binding = 1) uniform image2D output_PosData;//������Ŀ��ߵ����ͼ��
layout(r32i, binding = 2) uniform iimage2D input_ni;//ÿ�������ܵ���Լ����Ŀ
//uniform float w = 1;

void main(void)
{
	//��computeShader�Ĳ�������ΪԼ��������
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	vec4 PosDelta1 = imageLoad(input_PosDelta1, pos);
	//�ۼ�����Լ��������λ�õ�ƫ�ƣ���ͬ�ֵ�Լ�������ͻ
	vec4 originPos = imageLoad(output_PosData, pos);
	vec4 totalDelta = PosDelta1;
	//totalDelta = totalDelta + PosDelta2;

	//Successive Over-Relaxation
	int ni = int(imageLoad(input_ni, pos));
	//totalDelta = totalDelta / ni;//ni��Ӱ������ӵ�Լ����Ŀ
	//�����������Ŀ��ߵ�������
	imageStore(output_PosData, pos, originPos + totalDelta);
}