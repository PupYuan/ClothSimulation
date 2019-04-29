#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D input_PosDelta1;
layout(rgba32f, binding = 1) uniform image2D output_PosData;//粒子数目宽高的输出图像
layout(r32i, binding = 2) uniform iimage2D input_ni;//每个粒子受到的约束数目
uniform int width;			//size of position texture

void main(void)
{
	//该computeShader的并行数量为约束的数量
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	vec4 PosDelta1 = imageLoad(input_PosDelta1, pos);
	//累加所有约束对粒子位置的偏移，不同种的约束不会冲突
	vec4 originPos = imageLoad(output_PosData, pos);
	vec4 totalDelta = PosDelta1;
	//totalDelta = totalDelta + PosDelta2;

	//Successive Over-Relaxation
	float ni = float(imageLoad(input_ni, pos));
	totalDelta = totalDelta / ni;//ni是影响该粒子的约束数目
	if ((pos.x==0&&pos.y==0) || (pos.x==width-1&&pos.y==0))
		 totalDelta = vec4(0);
	//输出到粒子数目宽高的纹理中
	imageStore(output_PosData, pos, originPos + totalDelta);
}