#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(r32i, binding = 0) uniform iimage2D PosDeltaDataX;
layout(r32i, binding = 1) uniform iimage2D PosDeltaDataY;
layout(r32i, binding = 2) uniform iimage2D PosDeltaDataZ;

layout(rgba32f, binding = 3) uniform image2D output_PosData;//������Ŀ��ߵ����ͼ��
layout(r32i, binding = 4) uniform iimage2D input_ni;//ÿ�������ܵ���Լ����Ŀ

uniform int width;			//size of position texture

// ������ײ���������λ��
uniform vec3 sphere_pos[2];
// ������ײ����İ뾶����
uniform float radius[2];
uniform float w;//global user-parameter control the rate

void main(void)
{
	//��computeShader�Ĳ�������ΪԼ��������
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	float PosDeltaX = float(imageLoad(PosDeltaDataX, pos)/1000.0f);
	float PosDeltaY = float(imageLoad(PosDeltaDataY, pos)/1000.0f);
	float PosDeltaZ = float(imageLoad(PosDeltaDataZ, pos)/1000.0f);
	//�ۼ�����Լ��������λ�õ�ƫ�ƣ���ͬ�ֵ�Լ�������ͻ
	vec4 originPos = imageLoad(output_PosData, pos);
	vec4 totalDelta = vec4(PosDeltaX,PosDeltaY,PosDeltaZ,0);

	//Successive Over-Relaxation
	float ni = float(imageLoad(input_ni, pos));
	totalDelta = w*totalDelta / ni;//ni��Ӱ������ӵ�Լ����Ŀ
	if ((pos.x==0&&pos.y==0) || (pos.x==width-1&&pos.y==0))
		 totalDelta = vec4(0);

    //�����������ײ�жϺ���Ӧ
	vec4 finalPos = originPos + totalDelta;
	for(int i=0;i<2;i++){
	    vec3 v = finalPos.xyz - sphere_pos[i];
		if(length(v) < radius[i]){
			vec3 moveOffset  = normalize(v) * (radius[i] - length(v));
			finalPos  = finalPos + vec4(moveOffset,0);
		}
	}
	if(finalPos.y<-3.4)
	   finalPos.y=-3.4;
	//�����������Ŀ��ߵ�������
	imageStore(output_PosData, pos, finalPos);
}