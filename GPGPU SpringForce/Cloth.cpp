#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
float kBend = 0.5f;
float kStretch = 0.25f;

/* This is a important constructor for the entire system of particles and constraints*/
Cloth::Cloth(float _width, float _height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	total_points = (num_particles_width)*(num_particles_height);
	width = _width;
	height = _height;
	glfwSwapInterval(0);
	particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles
	vertices.resize(num_particles_width*num_particles_height * vertice_data_length);
	Ri.resize(total_points);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 0, ((float(y) / (num_particles_height - 1))* height));
			particles[y*num_particles_width + x] = Particle(pos, 1.0f / (num_particles_width * num_particles_height)); // insert particle in column x at y'th row
			Particle *particle = &particles[y*num_particles_width + x];

			//顶点位置,并且传递地址给particle
			vertices[vertice_data_length*(y*num_particles_width + x)] = pos.x;
			particle->x = &vertices[vertice_data_length * (y*num_particles_width + x)];

			vertices[vertice_data_length * (y*num_particles_width + x) + 1] = pos.y;
			particle->y = &vertices[vertice_data_length * (y*num_particles_width + x) + 1];

			vertices[vertice_data_length * (y*num_particles_width + x) + 2] = pos.z;
			particle->z = &vertices[vertice_data_length * (y*num_particles_width + x) + 2];
			//顶点法线，并且传递地址给particle
			vertices[vertice_data_length * (y*num_particles_width + x) + 3] = 0.0f;
			particle->normal_x = &vertices[vertice_data_length * (y*num_particles_width + x) + 3];

			vertices[vertice_data_length * (y*num_particles_width + x) + 4] = 0.0f;
			particle->normal_y = &vertices[vertice_data_length * (y*num_particles_width + x) + 4];

			vertices[vertice_data_length * (y*num_particles_width + x) + 5] = 1.0f;
			particle->normal_z = &vertices[vertice_data_length * (y*num_particles_width + x) + 5];
			//纹理坐标
			//x
			float texture_val_x;
			if (x % 2 == 0)
				texture_val_x = 0.0f;//左
			else
				texture_val_x = 1.0f;//右边
			vertices[vertice_data_length * (y*num_particles_width + x) + 6] = texture_val_x;
			//y
			float texture_val_y;
			if (y % 2 == 0)
				texture_val_y = 1.0f;//上
			else
				texture_val_y = 0.0f;//左上
			vertices[vertice_data_length * (y*num_particles_width + x) + 7] = texture_val_y;


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

	renderShader = ResourcesManager::loadShader("ClothShader", "../Resource/Shader/Simple.vs", "../Resource/Shader/Simple.fs");
	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 2; i++)
	{
		getParticle(0 + i, 0)->offsetPos(vec3(0.5, 0.0, 0.0));
		getParticle(0 + i, 0)->makeUnmovable();

		getParticle(num_particles_width - 1 - i, 0)->offsetPos(vec3(-0.5, 0.0, 0.0));
		getParticle(num_particles_width - 1 - i, 0)->makeUnmovable();

		getParticle(0 + i, num_particles_height - 1)->offsetPos(vec3(0.5, 0.0, 0.0));
		getParticle(0 + i, num_particles_height - 1)->makeUnmovable();

		getParticle(num_particles_width - 1 - i, num_particles_height - 1)->offsetPos(vec3(-0.5, 0.0, 0.0));
		getParticle(num_particles_width - 1 - i, num_particles_height - 1)->makeUnmovable();

		getParticle(0 + i, num_particles_height / 2 - 1)->offsetPos(vec3(0.5, 0.0, 0.0));
		getParticle(0 + i, num_particles_height / 2 - 1)->makeUnmovable();

		getParticle(num_particles_width - 1 - i, num_particles_height / 2 - 1)->offsetPos(vec3(-0.5, 0.0, 0.0));
		getParticle(num_particles_width - 1 - i, num_particles_height / 2 - 1)->makeUnmovable();
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertice_data_length * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertice_data_length * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertice_data_length * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Distance Constraints
			if (x + 1 < num_particles_width)
				AddConstraint(getParticle(x, y), getParticle(x + 1, y), kStretch);
			if (y + 1 < num_particles_height)
				AddConstraint(getParticle(x, y), getParticle(x, y + 1), kStretch);
			//Shear Springs
			if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
				AddConstraint(getParticle(x, y), getParticle(x + 1, y + 1), kStretch);
				AddConstraint(getParticle(x + 1, y), getParticle(x, y + 1), kStretch);
			}
		}
	}
	//add vertical constraints
	for (int i = 0; i < num_particles_width; i++) {
		for (int j = 0; j < num_particles_height - 2; j++) {
			BendingConstraint2* constraint = new BendingConstraint2(getParticle(i, j), getParticle(i, j + 1), getParticle(i, j + 2), kBend);
			Constraints.push_back(constraint);
		}
	}
	//add horizontal constraints
	for (int i = 0; i < num_particles_width - 2; i++) {
		for (int j = 0; j < num_particles_height; j++) {
			BendingConstraint2* constraint = new BendingConstraint2(getParticle(i, j), getParticle(i + 1, j), getParticle(i + 2, j), kBend);
			Constraints.push_back(constraint);
		}
	}
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
	//每次步进都会加重力
	addForce(SceneManager::gravity);
	glm::vec3 Xcm = glm::vec3(0);
	glm::vec3 Vcm = glm::vec3(0);
	float sumM = 0;
	//摩擦力
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		glm::vec3 V = GetVerletVelocity((*particle).getPos(), (*particle).getLastPos(), dt);
        //(*particle).addForce(DEFAULT_DAMPING * V);
        //直接按比例减少速度模拟摩擦力
		//glm::vec3 V = particle->getVelocity();
        V *= global_dampening;
		vec3 acc = particle->getAcceleration();
		V = V + acc * dt;
        particle->setVelocity(V);

		Xcm += ((*particle).getPos() * (*particle).getMass());
		Vcm += ((*particle).getVelocity() * (*particle).getMass());
		sumM += (*particle).getMass();
	}
	Xcm /= sumM;
	Vcm /= sumM;
	glm::mat3 I = glm::mat3(1);
	glm::vec3 L = glm::vec3(0);
	glm::vec3 w = glm::vec3(0);//angular velocity
	int i = 0;
	for (particle = particles.begin(); particle != particles.end(); particle++) {
		vec3 Xi = particle->getCurrentPos();
		Ri[i] = (Xi - Xcm);
		L += cross(Ri[i], particle->getVelocity() * particle->getMass());

		glm::mat3 tmp = glm::mat3(0, -Ri[i].z, Ri[i].y,
			Ri[i].z, 0, -Ri[i].x,
			-Ri[i].y, Ri[i].x, 0);
		I += (tmp*glm::transpose(tmp))*particle->getMass();
		i++;
	}
	w = glm::inverse(I)*L;
	i = 0;
	for (particle = particles.begin(); particle != particles.end(); i++, particle++) {
		glm::vec3 delVi = Vcm + glm::cross(w, Ri[i]) - particle->getVelocity();
		glm::vec3 V = particle->getVelocity();
		V += kDamp * delVi;
		particle->setVelocity(V);
		(*particle).timeStep(dt);
	}
	for (size_t si = 0; si < Constraint::solver_iterations; ++si) {
		std::vector<Constraint*>::iterator constraint;
		for (constraint = Constraints.begin(); constraint != Constraints.end(); constraint++)
		{
			(*constraint)->satisfyConstraint(dt); // satisfy Spring.
		}
	}
	GroundCollision();
}
void Cloth::GroundCollision()
{
	for (auto particle = particles.begin(); particle != particles.end(); particle++)
	{
		particle->checkGround(-10);
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

void Cloth::AddConstraint(Particle* a, Particle* b, float k) {
	DistanceConstraint* constraint = new DistanceConstraint();
	constraint->particle1 = a;
	constraint->particle2 = b;
	constraint->k = k;
	constraint->k_prime = 1.0f - pow((1.0f - k), 1.0f / Constraint::solver_iterations);
	if (constraint->k_prime > 1.0)
		constraint->k_prime = 1.0;

	glm::vec3 deltaP = vec3(a->getPos() - b->getPos());
	constraint->restDistance = sqrt(glm::dot(deltaP, deltaP));
	Constraints.push_back(constraint);
}

void Cloth::render()
{
	//场景的信息要单独抽离出来
	renderShader->use();
	renderShader->setMat4("projection", scene->projection);
	renderShader->setMat4("view", scene->view);
	glm::mat4 model1;
	//model1 = glm::translate(model1, glm::vec3(-6.0f, 2.0f, -5.0f)); // translate it down so it's at the center of the scene
	renderShader->setMat4("model", model1);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene->diffuseMap);

	renderShader->setVec3("light.position", scene->light.lightPos);
	renderShader->setVec3("viewPos", scene->camera.Position);
	renderShader->setVec3("light.ambient", scene->light.ambientColor);
	renderShader->setVec3("light.diffuse", scene->light.diffuseColor);
	renderShader->setVec3("light.specular", 0.2f, 0.2f, 0.2f);

	renderShader->setFloat("material.shininess", 16.0f);
	renderShader->setVec3("material.specular", vec3(0.2f, 0.2f, 0.2f));

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


