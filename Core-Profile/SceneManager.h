#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
Shader * ourShader;
Model *ourModel;
Cloth * cloth;
Shader * ClothShader;
unsigned int diffuseMap;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void SceneInit() {
	// build and compile shaders
	// -------------------------
	ourShader = new Shader("./Shader/lightingMaps.vs", "./Shader/lightingMaps.fs");
	ClothShader = new Shader("./Shader/Simple.vs", "./Shader/Simple.fs");
	//load Textures
	diffuseMap = loadTexture("../Resource/Textures/ClothTextures.png");
	// load models
	// -----------
	ourModel = new Model("./Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");

	cloth = new Cloth(14, 10, 55, 45);
}

float ball_time = 0;
vec3 ball_pos;

void RenderScene() {
	// don't forget to enable shader before setting uniforms
	ourShader->use();
	ourShader->setVec3("light.position", lightPos);
	ourShader->setVec3("viewPos", camera.Position);

	// light properties
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
	ourShader->setVec3("light.ambient", ambientColor);
	ourShader->setVec3("light.diffuse", diffuseColor);
	ourShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	ourShader->setFloat("material.shininess", 32.0f);


	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	ourShader->setMat4("projection", projection);
	ourShader->setMat4("view", view);

	// render the loaded model
	glm::mat4 model;
	model = glm::translate(model, ball_pos); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	ourShader->setMat4("model", model);
	ourModel->Draw(*ourShader);

	ClothShader->use();
	ClothShader->setMat4("projection", projection);
	ClothShader->setMat4("view", view);
	glm::mat4 model1;
	//model1 = glm::translate(model1, glm::vec3(-6.0f, 2.0f, -5.0f)); // translate it down so it's at the center of the scene
	ClothShader->setMat4("model", model1);
	ClothShader->setVec3("light.position", lightPos);
	ClothShader->setVec3("viewPos", camera.Position);
	ClothShader->setVec3("light.ambient", ambientColor);
	ClothShader->setVec3("light.diffuse", diffuseColor);
	ClothShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	ClothShader->setFloat("material.shininess", 32.0f);
	ClothShader->setVec3("material.diffuse", vec3(0.8f, 0.5f, 1.0f));
	ClothShader->setVec3("material.specular", vec3(0.8f, 0.5f, 1.0f));

	cloth->drawShaded();
}
