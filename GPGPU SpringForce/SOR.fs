#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(r32i, binding = 0) uniform iimage2D PosDeltaDataX;
layout(r32i, binding = 1) uniform iimage2D PosDeltaDataY;
layout(r32i, binding = 2) uniform iimage2D PosDeltaDataZ;

layout(rgba32f, binding = 3) uniform image2D output_PosData;//粒子数目宽高的输出图像
layout(r32i, binding = 4) uniform iimage2D input_ni;//每个粒子受到的约束数目
layout(rgba32f, binding = 5) uniform image2D Normal;//粒子数目宽高的输出图像
uniform int width;			//size of position texture

// 传递碰撞球体的球心位置
uniform vec3 sphere_pos[2];
// 传递碰撞球体的半径数据
uniform float radius[2];
uniform float w;//global user-parameter control the rate

void main(void)
{
	//该computeShader的并行数量为约束的数量
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	float PosDeltaX = float(imageLoad(PosDeltaDataX, pos)/1000.0f);
	float PosDeltaY = float(imageLoad(PosDeltaDataY, pos)/1000.0f);
	float PosDeltaZ = float(imageLoad(PosDeltaDataZ, pos)/1000.0f);
	//累加所有约束对粒子位置的偏移，不同种的约束不会冲突
	vec4 originPos = imageLoad(output_PosData, pos);
	vec4 totalDelta = vec4(PosDeltaX,PosDeltaY,PosDeltaZ,0);

	//Successive Over-Relaxation
	float ni = float(imageLoad(input_ni, pos));
	totalDelta = w*totalDelta / ni;//ni是影响该粒子的约束数目
	if ((pos.x==0&&pos.y==0) || (pos.x==width-1&&pos.y==0))
		 totalDelta = vec4(0);

	//法线的计算
	vec4 normal = vec4(0);
	imageStore(Normal, pos, normal);//清空一遍

	//第一块三角形的法线
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
     //第二块三角形的法线
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
	//在这里进行碰撞判断和响应
	vec4 finalPos = originPos + totalDelta;
	for(int i=0;i<2;i++){
	    vec3 v = finalPos.xyz - sphere_pos[i];
		if(length(v) < radius[i]){
			vec3 moveOffset  = normalize(v) * (radius[i] - length(v));
			finalPos  = finalPos + vec4(moveOffset,0);
		}
	}
	//输出到粒子数目宽高的纹理中
	imageStore(output_PosData, pos, finalPos);
}