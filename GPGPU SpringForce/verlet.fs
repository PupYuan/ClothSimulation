#version 330 core
layout (location = 0) out vec4 CurrentPos;
layout (location = 1) out vec4 LastPos;

in vec2 TexCoords;

uniform sampler2D X;					//current position
uniform sampler2D X_last;			//previous position
uniform float dt;						//timeStep
uniform float DEFAULT_DAMPING;	//default velocity damping
uniform float mass;					//point's mass
uniform float texsize_x;			//size of position texture
uniform float texsize_y;
uniform float KsStruct,KdStruct,KsShear,KdShear,KsBend,KdBend;	//spring stiffness constants
uniform vec2  inv_cloth_size;		//size of a single patch in world space
uniform vec2  step;					//delta texture size
uniform vec3  gravity;				//gravitational force

vec2 getNextNeighbor(int n, out float ks, out float kd) { 
   //structural springs (adjacent neighbors)
   //        o
   //        |
   //     o--m--o
   //        |
   //        o
   if(n<4) {
       ks = KsStruct;
       kd = KdStruct;
   }
	if (n == 0)	return vec2( 1,  0);
	if (n == 1)	return vec2( 0, -1);
	if (n == 2)	return vec2(-1,  0);
	if (n == 3)	return vec2( 0,  1);
	
	//shear springs (diagonal neighbors)
	//     o  o  o
	//      \   /
	//     o  m  o
	//      /   \
	//     o  o  o
	if(n<8) {
       ks = KsShear;
       kd = KdShear;
   }
	if (n == 4) return vec2( 1,  -1);
	if (n == 5) return vec2( -1, -1);	
	if (n == 6) return vec2(-1,  1);
	if (n == 7) return vec2( 1,  1);
	
	//bend spring (adjacent neighbors 1 node away)
	//
	//o   o   o   o   o
	//        | 
	//o   o   |   o   o
	//        |   
	//o-------m-------o
	//        |  
	//o   o   |   o   o
	//        |
	//o   o   o   o   o 
	if(n<12) {
       ks = KsBend;
       kd = KdBend;
   }
	if (n == 8)	return vec2( 2, 0);
	if (n == 9) return vec2( 0, -2);
	if (n ==10) return vec2(-2, 0);
	if (n ==11) return vec2( 0, 2);
}

void main() {
	vec3 x_i = texture2D(X, TexCoords).xyz;
	vec3 x_last = texture2D(X_last, TexCoords).xyz;

	float mymass = mass; 
	vec3 vel	= (x_i - x_last) / dt;	// calc. velocity according to verlet integration
	float ix = floor(TexCoords.x * texsize_x);
	float iy = floor(TexCoords.y * texsize_y);
	float index = iy * texsize_x + ix;
	float ks=  0.0, kd= 0.0;

	//if (index==0 || index== (texsize_x - 1.0))
	//	 mymass = 0.0;
 
	vec3 force = gravity*mymass + vel*DEFAULT_DAMPING;



	vec3 acc;
	if(mymass == 0) 		
	   acc = vec3(0);	//prevent the explosion due to divide by zero
	else
	   acc = force/mymass;

	if(x_i.y<-10)
	   x_i.y=-10;

	// verlet integration
	vec3 tmp = x_i;
	x_i = x_i * 2.0 - x_last + acc * dt * dt;
	x_last = tmp;
	// fragment outputs
	CurrentPos = vec4(x_i,1.0);
	LastPos = vec4(x_last,0.0);
}