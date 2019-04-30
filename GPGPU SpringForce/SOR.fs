#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D input_PosDelta1;
layout(rgba32f, binding = 1) uniform image2D output_PosData;//������Ŀ��ߵ����ͼ��
layout(r32i, binding = 2) uniform iimage2D input_ni;//ÿ�������ܵ���Լ����Ŀ
layout(rgba32f, binding = 3) uniform image2D Normal;//������Ŀ��ߵ����ͼ��
uniform int width;			//size of position texture

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
	float ni = float(imageLoad(input_ni, pos));
	totalDelta = totalDelta / ni;//ni��Ӱ������ӵ�Լ����Ŀ
	if ((pos.x==0&&pos.y==0) || (pos.x==width-1&&pos.y==0))
		 totalDelta = vec4(0);

	//���ߵļ���
	vec4 normal = vec4(0);
	imageStore(Normal, pos, normal);//���һ��

	//��һ�������εķ���
	vec4 p1 = imageLoad(output_PosData, ivec2(pos.x+1,pos.y));
	vec4 p2 = imageLoad(output_PosData, ivec2(pos.x,pos.y));
	vec4 p3 = imageLoad(output_PosData, ivec2(pos.x,pos.y+1));
	vec4 v1 = p2-p1;
	vec4 v2 = p3-p1;
	normal = vec4(cross(v1.xyz,p2.xyz),1);
	imageStore(Normal, ivec2(pos.x+1,pos.y), normal + imageLoad(Normal, ivec2(pos.x+1,pos.y)));
	imageStore(Normal, ivec2(pos.x,pos.y), normal + imageLoad(Normal, ivec2(pos.x,pos.y)));
	imageStore(Normal, ivec2(pos.x,pos.y+1), normal + imageLoad(Normal, ivec2(pos.x,pos.y+1)));
	barrier();
     //�ڶ��������εķ���
	p1 = imageLoad(output_PosData, ivec2(pos.x+1,pos.y+1));
	p2 = imageLoad(output_PosData, ivec2(pos.x+1,pos.y));
	p3 = imageLoad(output_PosData, ivec2(pos.x,pos.y+1));
	v1 = p2-p1;
	v2 = p3-p1;
	normal = vec4(cross(v1.xyz,p2.xyz),1);
	imageStore(Normal, ivec2(pos.x+1,pos.y+1),normal + imageLoad(Normal, ivec2(pos.x+1,pos.y+1)));
	imageStore(Normal, ivec2(pos.x+1,pos.y),normal + imageLoad(Normal, ivec2(pos.x+1,pos.y)));
	imageStore(Normal, ivec2(pos.x,pos.y+1), normal + imageLoad(Normal, ivec2(pos.x,pos.y+1)));

	barrier();
	//�����������Ŀ��ߵ�������
	imageStore(output_PosData, pos, originPos + totalDelta);
}