#include "Cloth.h"
#include "Collider.h"
float	KsStruct = 50.75f, KdStruct = -0.25f;
float	KsShear = 50.75f, KdShear = -0.25f;
float	KsBend = 50.95f, KdBend = -0.25f;

/* This is a important constructor for the entire system of particles and constraints*/
Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles
	vertices.resize(num_particles_width*num_particles_height * 6);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			vec3 pos = vec3(width * (x / (float)num_particles_width),
				-height * (y / (float)num_particles_height),
				0);

			particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
			Particle *particle = &particles[y*num_particles_width + x];
			//同样保存一份在内存中：
			//顶点位置,并且传递地址给particle
			//
			vertices[6*(y*num_particles_width + x)] = pos.x;
			particle->x = &vertices[6 * (y*num_particles_width + x)];

			vertices[6 * (y*num_particles_width + x)+1] = pos.y;
			particle->y = &vertices[6 * (y*num_particles_width + x) + 1];

			vertices[6 * (y*num_particles_width + x) + 2] = pos.z;
			particle->z = &vertices[6 * (y*num_particles_width + x) + 2];
			//顶点法线
			vertices[6 * (y*num_particles_width + x) + 3] = 0.0f;
			vertices[6 * (y*num_particles_width + x) + 4] = 0.0f;
			vertices[6 * (y*num_particles_width + x) + 5] = 1.0f;
		}
	}
	//填充索引数据到indices中
	for (int y = 0; y < num_particles_height - 1; y++)
	{
		for (int x = 0; x < num_particles_width - 1; x++)
		{
			indices.push_back((y)*num_particles_width + x + 1);
			indices.push_back((y + 1)*num_particles_width + x + 1);
			indices.push_back((y)*num_particles_width + x);

			indices.push_back((y + 1)*num_particles_width + x + 1);
			indices.push_back((y + 1)*num_particles_width + x);
			indices.push_back((y)*num_particles_width + x);
		}
	}

	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 3; i++)
	{
		getParticle(0 + i, 0)->offsetPos(vec3(0.5, 0.0, 0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
		getParticle(0 + i, 0)->makeUnmovable();

		getParticle(0 + i, 0)->offsetPos(vec3(-0.5, 0.0, 0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
		getParticle(num_particles_width - 1 - i, 0)->makeUnmovable();
	}


	//初始化gl缓存对象
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vertices.size(), &vertices[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Constraints
		// Setup springs
	// 添加Springs
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Structure Springs
			if (x + 1 < num_particles_width)
				AddSpring(getParticle(x, y), getParticle(x + 1, y), KsStruct, KdStruct);
			if (y + 1 < num_particles_height)
				AddSpring(getParticle(x, y), getParticle(x, y + 1), KsStruct, KdStruct);

			//Shear Springs
			if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
				AddSpring(getParticle(x, y), getParticle(x + 1, y + 1), KsShear, KdShear);
				AddSpring(getParticle(x + 1, y), getParticle(x, y + 1), KsShear, KdShear);
			}

			//Bending Springs
			if (x + 2 < num_particles_width)
				AddSpring(getParticle(x, y), getParticle(x + 2, y), KsBend, KdBend);
			if (y + 2 < num_particles_height)
				AddSpring(getParticle(x, y), getParticle(x, y + 2), KsBend, KdBend);
			if (y + 2 < num_particles_height && x + 2 < num_particles_width) {
				AddSpring(getParticle(x, y), getParticle(x + 2, y + 2), KsBend, KdBend);
				AddSpring(getParticle(x + 2, y), getParticle(x, y + 2), KsBend, KdBend);
			}
		}
	}
}

void Cloth::drawShaded()
{
	// reset normals (which where written to last frame)
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).resetNormal();
	}

	//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{
			vec3 normal = calcTriangleNormal(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1));
			getParticle(x + 1, y)->addToNormal(normal);
			getParticle(x, y)->addToNormal(normal);
			getParticle(x, y + 1)->addToNormal(normal);

			normal = calcTriangleNormal(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1));
			getParticle(x + 1, y + 1)->addToNormal(normal);
			getParticle(x + 1, y)->addToNormal(normal);
			getParticle(x, y + 1)->addToNormal(normal);
		}
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vertices.size(), &vertices[0]);
	glDrawElements(GL_TRIANGLES, 6 * (num_particles_height - 1)*(num_particles_width - 1), GL_UNSIGNED_INT, 0);
}

inline glm::vec3 GetVerletVelocity(glm::vec3 x_i, glm::vec3 xi_last, float dt) {
	return  (x_i - xi_last) / dt;
}

void Cloth::addForce(const vec3 direction)
{
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).addForce(direction); // add the forces to each particle
	}
}

//物理模拟
void Cloth::timeStep(float dt) {
	//摩擦力
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		glm::vec3 V = GetVerletVelocity((*particle).getPos(), (*particle).getLastPos(), dt);
		(*particle).addForce(DEFAULT_DAMPING * V);
	}

	std::vector<Spring>::iterator Spring;
	for (Spring = Springs.begin(); Spring != Springs.end(); Spring++)
	{
		(*Spring).satisfySpring(dt); // satisfy Spring.
	}

	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).timeStep(dt);
	}
}

void Cloth::AddSpring(Particle* a, Particle* b, float ks, float kd) {
	Spring spring;
	spring.p1 = a;
	spring.p2 = b;
	spring.Ks = ks;
	spring.Kd = kd;
	glm::vec3 deltaP = vec3(a->getPos() - b->getPos());
	spring.restDistance = sqrt(glm::dot(deltaP, deltaP));
	Springs.push_back(spring);
}

void Cloth::CollisionDetection(Collider * collider)
{
	collider->ClothCollisionSimulate(this);
}