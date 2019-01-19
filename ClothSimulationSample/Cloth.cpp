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
			vec3 pos = vec3(width * (x / (float)num_particles_width),
				4,
				height * (y / (float)num_particles_height));
			particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
		}
	}
	// 添加Springs
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Structure Springs
			if (x + 1 < num_particles_width)
				makeSpring(getParticle(x, y), getParticle(x + 1, y));
			if (y + 1 < num_particles_height)
				makeSpring(getParticle(x, y), getParticle(x, y + 1));

			//Shear Springs
			if (y + 1 < num_particles_height && x + 1 < num_particles_width)
				makeSpring(getParticle(x, y), getParticle(x + 1, y + 1));
			if (y - 1 > 0 && x - 1 > 0)
				makeSpring(getParticle(x, y), getParticle(x - 1, y - 1));

			//Bending Springs
			if (x + 2 < num_particles_width)
				makeSpring(getParticle(x, y), getParticle(x + 2, y));
			if (y + 2 < num_particles_height)
				makeSpring(getParticle(x, y), getParticle(x, y + 2));
			if (y + 2 < num_particles_height && x + 2 < num_particles_width)
				makeSpring(getParticle(x, y), getParticle(x + 2, y + 2));
			if (y - 2 > 0 && x - 2 > 0)
				makeSpring(getParticle(x, y), getParticle(x - 2, y - 2));
		}
	}
	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 3; i++)
	{
		getParticle(0 + i, 0)->makeUnmovable();
		getParticle(num_particles_width - 1 - i, 0)->makeUnmovable();
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

void Cloth::drawShaded()
{
	// reset normals (which where written to last frame)
	std::vector<Particle>::iterator particle;
	//glBegin(GL_TRIANGLES);
	glBegin(GL_POINTS);
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{
			vec3 color(1, 0, 0);
			vec3 pos = getParticle(x, y)->getPos();
			glVertex3f(pos.x, pos.y, pos.z);

			//if (x % 2) // red and white color is interleaved according to which column number
			//	color = vec3(0.6f, 0.2f, 0.2f);
			//else
			//	color = vec3(1.0f, 1.0f, 1.0f);

			/*drawTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), color);
			drawTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), color);*/
		}
	}
	glEnd();
}

//物理模拟
void Cloth::timeStep(float dt) {

	std::vector<Spring>::iterator Spring;
	for (int i = 0; i < Spring_ITERATIONS; i++) // iterate over all Springs several times
	{
		for (Spring = Springs.begin(); Spring != Springs.end(); Spring++)
		{
			(*Spring).satisfySpring(dt); // satisfy Spring.
		}
	}

	std::vector<Particle>::iterator particle;
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

////碰撞检测
//void Cloth::CollisionDetection(Collider * collider) {
//	collider->ClothCollisionSimulate(this);
//}