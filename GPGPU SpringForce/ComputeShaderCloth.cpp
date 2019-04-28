#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\util.h>

void ComputeShaderCloth::timeStep(float dt)
{
	CHECK_GL_ERRORS
	for (int i = 0; i < NUM_ITER; i++) {
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		DistanceConstraintCompute->use();
		glFinish();
		glBindImageTexture(0, attachID[2*readID], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, DistanceTexID1, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32I);
		glBindImageTexture(2, DistanceTexID2, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32I);
		glBindImageTexture(3, DistanceDeltaTexID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(DistanceConstraintIndexData1.size(), 1, 1);
		glFinish();

		SuccessiveOverRelaxationCompute->use();
		glFinish();
		glBindImageTexture(0, DistanceDeltaTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, attachID[2*readID], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(2, NiTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I);
		glDispatchCompute(num_particles_width, num_particles_height, 1);
		glFinish();
		//swap read/write pathways
		int tmp = readID;
		readID = writeID;
		writeID = tmp;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID[readID]);
	//��framebuffer�е���ɫ������ȡ��
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, vboID);
	glReadPixels(0, 0, num_particles_width, num_particles_height, GL_RGBA, GL_FLOAT, 0);

	CHECK_GL_ERRORS
	//����״̬
	glReadBuffer(GL_NONE);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	glViewport(0, 0, scene->SCR_WIDTH, scene->SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
}

/**
 * Sets up a floating point texture with the NEAREST filtering.
 */
void setupTexture(const GLuint texID, float *data,int width,int height) {
	// make active and bind
	glBindTexture(GL_TEXTURE_2D, texID);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, 16, 16);
	// turn off filtering and wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0,
		GL_RGBA, GL_FLOAT, data);

}

ComputeShaderCloth::ComputeShaderCloth(float _width, float _height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	width = _width;
	height = _height;
	glfwSwapInterval(0);
	X.resize(total_points);
	X_last.resize(total_points);
	Normal.resize(total_points);
	TexCoord.resize(total_points);
	Ni.resize(total_points,0);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 1.0, ((float(y) / (num_particles_height - 1))* height));

			X[(y*num_particles_width + x)] = vec4(pos, 1);
			X_last[(y*num_particles_width + x)] = vec4(pos, 1);
			Normal[(y*num_particles_width + x)] = vec3(0, 0, 1);
			//��������
			//x
			vec2 temp(0, 0);
			float texture_val_x;
			if (x % 2 == 0)
				texture_val_x = 0.0f;//��
			else
				texture_val_x = 1.0f;//�ұ�
			temp.x = texture_val_x;
			//y
			float texture_val_y;
			if (y % 2 == 0)
				texture_val_y = 1.0f;//��
			else
				texture_val_y = 0.0f;//����
			temp.y = texture_val_y;
			TexCoord[(y*num_particles_width + x)] = temp;
		}
	}
	//����������ݵ�indices��
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

	//���Լ������
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Distance Constraints
			if (x + 1 < num_particles_width) {
				DistanceConstraintIndexData1.push_back(i32vec2(x, y));
				Ni[(y*num_particles_width + x)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x+1, y));
				Ni[(y*num_particles_width + x + 1)]++;
			}
				
			if (y + 1 < num_particles_height) {
				DistanceConstraintIndexData1.push_back(i32vec2(x, y));
				Ni[(y*num_particles_width + x)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x, y+1));
				Ni[((y+1)*num_particles_width + x)]++;
			}
			//Shear Springs
			if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
				DistanceConstraintIndexData1.push_back(i32vec2(x, y));
				Ni[((y)*num_particles_width + x)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x+1, y + 1));
				Ni[((y+1)*num_particles_width + x+1)]++;

				DistanceConstraintIndexData1.push_back(i32vec2(x+1, y));
				Ni[((y)*num_particles_width + x + 1)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x, y + 1));
				Ni[((y+1)*num_particles_width + x)]++;
			}
		}
	}

	renderShader = ResourcesManager::loadShader("GPU_renderShader", "render.vs", "render.fs");
	computeShader = ResourcesManager::loadComputeShader("ParticleSimulation", "ParticleSimulation.fs");
	DistanceConstraintCompute = ResourcesManager::loadComputeShader("DistanceConstraint", "DistanceConstraint.fs");
	SuccessiveOverRelaxationCompute = ResourcesManager::loadComputeShader("SOR", "SOR.fs");

	const int size = num_particles_width * num_particles_height * 4 * sizeof(float);
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &EBO);
	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_DYNAMIC_DRAW);
	//����λ������
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	//���㷨������
	glGenBuffers(1, &vboID2);
	glBindBuffer(GL_ARRAY_BUFFER, vboID2);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 3 * sizeof(float), &Normal[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	//������������
	glGenBuffers(1, &vboID3);
	glBindBuffer(GL_ARRAY_BUFFER, vboID3);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 2 * sizeof(float), &TexCoord[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_data[0] = &X[0].x;
	_data[1] = &X_last[0].x;

	// framebuffer configuration
	// -------------------------
	glGenFramebuffers(2, fboID);
	glGenTextures(4, attachID);
	for (int j = 0; j < 2; j++) {//����֡���壬����������������
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[j]);
		for (int i = 0; i < 2; i++) {//������������verlet���ֵĵ�ǰλ�ú͹�ȥλ��
			setupTexture(attachID[i+2*j], _data[i], num_particles_width, num_particles_height);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, mrt[i], GL_TEXTURE_2D, attachID[i + 2 * j], 0);
		}
		//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, attachID[j], 0);
	}
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status == GL_FRAMEBUFFER_COMPLETE) {
		printf("FBO setup succeeded.");
	}
	else {
		printf("Problem with FBO setup.");
	}
	glCheckError();

	glGenTextures(1, &DistanceTexID1);
	glGenTextures(1, &DistanceTexID2);
	
	//DistanceConstraint����洢��Ĭ��֡������?
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, DistanceTexID1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, DistanceConstraintIndexData1.size(), 1, 0,
		GL_RG_INTEGER, GL_INT, &DistanceConstraintIndexData1[0].x);
	glCheckError();

	glBindTexture(GL_TEXTURE_2D, DistanceTexID2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, DistanceConstraintIndexData2.size(), 1, 0,
		GL_RG_INTEGER, GL_INT, &DistanceConstraintIndexData2[0].x);
	//�洢DistanceConstraintԼ��������������������Ϊ������Ŀ���
	glGenTextures(1, &DistanceDeltaTexID);
	setupTexture(DistanceDeltaTexID, nullptr, num_particles_width, num_particles_height);//attachID�����Ŷ�������

	glGenTextures(1, &NiTexID);
	glBindTexture(GL_TEXTURE_2D, NiTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, num_particles_width, num_particles_height, 0,
		GL_RED_INTEGER, GL_INT, &Ni[0]);
	//glCheckError();
}

void ComputeShaderCloth::render()
{
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

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, 6 * (num_particles_height - 1)*(num_particles_width - 1), GL_UNSIGNED_INT, 0);
	CHECK_GL_ERRORS
}