#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <ClothSimulation\SceneManager.h>

//�ڳ����п���Ⱦ��ʵ��
class Renderable {
protected:
	static GLfloat near_plane, far_plane;
	// Configure depth map FBO
	static const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	//��Ȼ���ÿ������Ⱦ����ֻ��Ҫһ��
	static GLuint depthMapFBO;
	static GLuint depthMap;
	static Shader *debugDepthQuad;
	Shader *DepthShader;//���ڼ�����Ӱ�����shader
	Shader *shader;
	SceneManager* scene;
public:
	//��Ȼ��壬ÿ������ֻ��Ҫһ��
	static void DepthInit();
	glm::vec3 pos = vec3(0,0,0);
	glm::vec3 scale = vec3(1,1,1);
	virtual void render(Shader*shader);//render����PASS�����ε���draw();
	virtual void draw();
	SphereCollider* ball_collider;
	Renderable() {
		shader = ResourcesManager::loadShader("lightingMaps", "lightingMaps.vs", "lightingMaps.fs");
		DepthShader = ResourcesManager::loadShader("simpleDepthShader", "simpleDepthShader.vs", "simpleDepthShader.fs");

		shader->use();
		shader->setInt("material.texture_diffuse1", 0);
		shader->setInt("material.texture_specular1", 1);
		shader->setInt("shadowMap", 2);
	}
};

class ShpereRenderable:public Renderable {
public:
	//��ʱ��Model����
	Model *model;
	ShpereRenderable(SceneManager* _scene) {
		scene = _scene;
		shader = ResourcesManager::loadShader("lightingMaps", "lightingMaps.vs", "lightingMaps.fs");

		model = new Model("../Resource/Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");
	}
	virtual void render(Shader*shader);
};

class MeshRenderer:public Renderable {
	//��ʱ��Model����
	Model *model;
	MeshRenderer(SceneManager* _scene) {
		scene = _scene;
	}
	virtual void render(Shader*shader);
};
class Plane :public Renderable {
public:
	
	GLuint planeVAO;
	GLuint woodTexture;
	
	Plane(SceneManager * _scene);
	virtual void render(Shader*shader);
};
//������Ⱦframebuffer color targets�ܺ�
class Cube :public Renderable {
	static GLuint cubeVAO;
	static GLuint cubeVBO;
public :
	Cube(SceneManager * _scene);
	virtual void render(Shader*shader) {
		// Render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
};

//������Ⱦframebuffer color targets�ܺ�
class Quad :public Renderable {
	static GLuint quadVAO;
	static GLuint quadVBO;
public:
	Quad(SceneManager * _scene);
	void render(Shader*shader);
};