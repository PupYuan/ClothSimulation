#version 430 core
layout (local_size_x = 1024, local_size_y = 1) in;

layout (rgba32f, binding = 1) uniform image2D input_image;
layout (rgba32f, binding = 2) uniform image2D output_image;
//λ��ƫ���λ������
layout (rgba32f, binding = 3) uniform image2D C1Delta;
layout (rgba32f, binding = 4) uniform image2D C1Index;
layout (rgba32f, binding = 5) uniform image2D C2Delta;
layout (rgba32f, binding = 6) uniform image2D C2Index;

//���������Ŀ������Ϊ1024*1024
shared vec4 PosTemp[1024][1024];
//Ŀǰ������Լ��
void main(){
   ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
   ivec2 index1 = imageLoad(C1Index,pos);
   ivec2 index2 = imageLoad(C2Index,pos);
   PosTemp[index1.x][index1.y] = imageLoad(input_image,index1);
   PosTemp[index2.x][index2.y] = imageLoad(input_image,index2);
   barrier();//ͬ��

   //����ԭ�Ӳ���
   atomicAdd(PosTemp[index1.x][index1.y],imageLoad(C1Delta, index1));
   atomicAdd(PosTemp[index2.x][index2.y],imageLoad(C2Delta, index2));

   //�洢
   imageStore(output_image, index1, PosTemp[index1.x][index1.y]);
   imageStore(output_image, index2, PosTemp[index2.x][index2.y]); 
}