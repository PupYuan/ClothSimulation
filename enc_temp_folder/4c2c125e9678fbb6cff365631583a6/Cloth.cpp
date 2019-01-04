#include "Cloth.h"
#include "Collider.h"

Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles

	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			Vec3 pos = Vec3(width * (x / (float)num_particles_width),
				-height * (y / (float)num_particles_height),
				0);
			particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
		}
	}
	// 添加constraints
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Structure constraints
			if (x + 1 < num_particles_width)
				makeConstraint(getParticle(x, y), getParticle(x + 1, y));
			if (y + 1 < num_particles_height)
				makeConstraint(getParticle(x, y), getParticle(x, y + 1));

			//Shear constraints
			if (y + 1 < num_particles_height && x + 1 < num_particles_width)
				makeConstraint(getParticle(x, y), getParticle(x + 1, y + 1));
			if (y - 1 > 0 && x - 1 > 0)
				makeConstraint(getParticle(x, y), getParticle(x - 1, y - 1));

			//Bending constraints
			if (x + 2 < num_particles_width)
				makeConstraint(getParticle(x, y), getParticle(x + 2, y));
			if (y + 2 < num_particles_height)
				makeConstraint(getParticle(x, y), getParticle(x, y + 2));
			if (y + 2 < num_particles_height && x + 2 < num_particles_width)
				makeConstraint(getParticle(x, y), getParticle(x + 2, y + 2));
			if (y - 2 > 0 && x - 2 > 0)
				makeConstraint(getParticle(x, y), getParticle(x - 2, y - 2));
		}
	}
	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 3; i++)
	{
		getParticle(0 + i, 0)->makeUnmovable();
		getParticle(num_particles_width - 1 - i, 0)->makeUnmovable();
	}
}

void Cloth::drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color)
{
	glColor3fv((GLfloat*)&color);

	glNormal3fv((GLfloat *) &(p1->getNormal().normalized()));
	glVertex3fv((GLfloat *) &(p1->getPos()));

	glNormal3fv((GLfloat *) &(p2->getNormal().normalized()));
	glVertex3fv((GLfloat *) &(p2->getPos()));

	glNormal3fv((GLfloat *) &(p3->getNormal().normalized()));
	glVertex3fv((GLfloat *) &(p3->getPos()));
}

void Cloth::drawShaded()
{
	// reset normals (which where written to last frame)
	std::vector<Particle>::iterator particle;
	glBegin(GL_TRIANGLES);
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{
			Vec3 color(0, 0, 0);
			if (x % 2) // red and white color is interleaved according to which column number
				color = Vec3(0.6f, 0.2f, 0.2f);
			else
				color = Vec3(1.0f, 1.0f, 1.0f);

			drawTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), color);
			drawTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), color);
		}
	}
	glEnd();
}

//物理模拟
void Cloth::timeStep() {

	std::vector<Constraint>::iterator constraint;
	for (int i = 0; i < CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
	{
		for (constraint = constraints.begin(); constraint != constraints.end(); constraint++)
		{
			(*constraint).satisfyConstraint(); // satisfy constraint.
		}
	}

	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).timeStep();
	}
}

void Cloth::CollisionDetection(Collider * collider)
{
	collider->ClothCollisionSimulate(this);
}

void Cloth::addForce(const Vec3 direction)
{
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).addForce(direction); // add the forces to each particle
	}

}

////碰撞检测
//void Cloth::CollisionDetection(Collider * collider) {
//	collider->ClothCollisionSimulate(this);
//}