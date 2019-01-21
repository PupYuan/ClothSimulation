#include "Cloth.h"
#include "Collider.h"

float	KsStruct = 50.75f, KdStruct = -0.25f;
float	KsShear = 50.75f, KdShear = -0.25f;
float	KsBend = 50.95f, KdBend = -0.25f;

Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	sizeX = width;
	sizeY = height;
	int i = 0, j = 0, count = 0;
	int l1 = 0, l2 = 0;
	int v = num_particles_height + 1;
	int u = num_particles_width + 1;

	particles.resize(total_points);
	indices.resize(num_particles_width*num_particles_height * 2 * 3);

	//fill in positions
	for (j = 0; j <= num_particles_height; j++) {
		for (i = 0; i <= num_particles_width; i++) {
			vec3 pos = glm::vec3(((float(i) / (u - 1)) * 2 - 1)* hsize, sizeX + 1, ((float(j) / (v - 1))* sizeY));
			particles[count] = Particle(pos); // insert particle in column x at y'th row
			count++;
		}
	}
	//fill in indices
	GLushort* id = &indices[0];
	for (i = 0; i < num_particles_height; i++) {
		for (j = 0; j < num_particles_width; j++) {
			int i0 = i * (num_particles_width + 1) + j;
			int i1 = i0 + 1;
			int i2 = i0 + (num_particles_width + 1);
			int i3 = i2 + 1;
			if ((j + i) % 2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}
	// Setup springs
    // 添加Springs
	for (int x = 0; x <= num_particles_width; x++)
	{
		for (int y = 0; y <= num_particles_height; y++)
		{
			// Structure Springs
			if (x + 1 <= num_particles_width)
				AddSpring(getParticle(x, y), getParticle(x + 1, y), KsStruct, KdStruct);
			if (y + 1 <= num_particles_height)
				AddSpring(getParticle(x, y), getParticle(x, y + 1), KsStruct, KdStruct);

			//Shear Springs
			if (y + 1 <= num_particles_height && x + 1 <= num_particles_width) {
				AddSpring(getParticle(x, y), getParticle(x + 1, y + 1), KsShear, KdShear);
				AddSpring(getParticle(x + 1, y), getParticle(x, y + 1), KsShear, KdShear);
			}
		
			//Bending Springs
			if (x + 2 <= num_particles_width)
				AddSpring(getParticle(x, y), getParticle(x + 2, y), KsBend, KdBend);
			if (y + 2 <= num_particles_height)
				AddSpring(getParticle(x, y), getParticle(x, y + 2), KsBend, KdBend);
			if (y + 2 <= num_particles_height && x + 2 <= num_particles_width) {
				AddSpring(getParticle(x, y), getParticle(x + 2, y + 2), KsBend, KdBend);
				AddSpring(getParticle(x+2, y), getParticle(x, y + 2), KsBend, KdBend);
			}
		}
	}
	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 3; i++)
	{
		getParticle(0 + i, 0)->makeUnmovable();
		getParticle(num_particles_width - i, 0)->makeUnmovable();
	}
}

void Cloth::drawTriangle(Particle *p1, Particle *p2, Particle *p3, const vec3 color)
{
	glColor3fv((GLfloat*)&color);

	glNormal3fv((GLfloat *) &(normalize(p1->getNormal())));
	glVertex3fv((GLfloat *) &(p1->getPos()));

	glNormal3fv((GLfloat *) &(normalize(p2->getNormal())));
	glVertex3fv((GLfloat *) &(p2->getPos()));

	glNormal3fv((GLfloat *) &(normalize(p3->getNormal())));
	glVertex3fv((GLfloat *) &(p3->getPos()));
}

void Cloth::draw()
{
	//画布料
	switch (current_mode) {
	case CPU:
		RenderCPU();
		break;

	case GPU:
		RenderGPU();
		break;
	}
}

void Cloth::RenderCPU() {
	//draw polygons
	glColor3f(1, 1, 1);
	int i;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < indices.size(); i += 3) {
		glm::vec3 p1 = particles[indices[i]].getPos();
		glm::vec3 p2 = particles[indices[i + 1]].getPos();
		glm::vec3 p3 = particles[indices[i + 2]].getPos();
		/*glm::vec3 p1 = vec3(X[indices[i]]);
		glm::vec3 p2 = vec3(X[indices[i + 1]]);
		glm::vec3 p3 = vec3(X[indices[i + 2]]);*/
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
	}
	glEnd();

	//draw points	
	glBegin(GL_POINTS);
	for (i = 0; i < total_points; i++) {
		//glm::vec3 p = vec3(X[i]);
		glm::vec3 p = particles[i].getPos();
		int is = (i == selected_index);
		glColor3f((float)!is, (float)is, (float)is);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();
}

void Cloth::RenderGPU()
{
}

inline glm::vec3 GetVerletVelocity(glm::vec3 x_i, glm::vec3 xi_last, float dt) {
	return  (x_i - xi_last) / dt;
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

void Cloth::CollisionDetection(Collider * collider)
{
	collider->ClothCollisionSimulate(this);
}

void Cloth::addForce(const vec3 direction)
{
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).addForce(direction); // add the forces to each particle
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