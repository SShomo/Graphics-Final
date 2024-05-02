#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <bob/framebuffer.h>

void resetCamera(ew::Camera* camera, ew::CameraController* controller);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
ew::Transform monkeyTransform;
ew::Camera camera;
ew::CameraController cameraController;

float rimThres = 5.4f;
float rimCut = 0.1f;
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 objColor = glm::vec3(1.0f);
glm::vec3 lightOrbitCenter = glm::vec3(0, 3.0f, 0);
float lightOrbitRadius = 5.0f;
float lightOrbitSpeed = 1.0f;

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

float blurEffect = 1.0f;
int kernal;

int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float gamma = 1.0f;
float deltaTime;

glm::vec3 edgeColor = glm::vec3(0.0f);
float outlineX = 1.0;
float outlineY = 1.0;
float offset = 1.0;

int main() {

	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	bob::Framebuffer framebuffer = bob::createFramebufferWithRBO(screenWidth, screenHeight, GL_RGB16F);
	ew::Shader ppShader = ew::Shader("assets/pp.vert", "assets/pp.frag");
	ew::Shader outlineShader = ew::Shader("assets/outline.vert", "assets/outline.frag");
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");

	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Model bunnyModel = ew::Model("assets/bunny.obj");

	ew::Transform monkeyTransform;
	ew::Transform sandTransform;
	ew::Transform christmasTransform;
	ew::Transform porceTransform;
	ew::Transform rockTransform;
	ew::Transform concreteTransform;
	ew::Transform tileTransform;

	monkeyTransform.position = glm::vec3(0.0f, 5.0f, 5.0f);
	sandTransform.position = glm::vec3(3.0f, 0.0f, 0.0f);
	christmasTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	porceTransform.position = glm::vec3(0.0f, 3.0f, 0.0f);
	rockTransform.position = glm::vec3(-6.0f, 0.0f, 0.0f);
	concreteTransform.position = glm::vec3(6.0f, 0.0f, 0.0f);
	tileTransform.position = glm::vec3(-3.0f, 0.0f, 0.0f);

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing

	GLuint christmasNormalGL = ew::loadTexture("assets/christmas_normal_gl.jpg");
	GLuint christmasColor = ew::loadTexture("assets/christmas_color.jpg");

	GLuint porceNormalGL = ew::loadTexture("assets/porcelain_normal_gl.jpg");
	GLuint porceColor = ew::loadTexture("assets/porcelain_color.jpg");

	GLuint rockNormalGL = ew::loadTexture("assets/rock_normal_gl.jpg");
	GLuint rockColor = ew::loadTexture("assets/rock_color.jpg");

	GLuint concreteColor = ew::loadTexture("assets/concrete_color.jpg");
	GLuint concreteNormalGL = ew::loadTexture("assets/concrete_normal_gl.jpg");

	GLuint sandColor = ew::loadTexture("assets/sand_color.jpg");
	GLuint sandNormalGL = ew::loadTexture("assets/sand_normal_gl.jpg");

	GLuint tileColor = ew::loadTexture("assets/tile_color.jpg");
	GLuint tileNormalGL = ew::loadTexture("assets/tile_normal_gl.jpg");

	shader.use();
	shader.setInt("_MainTex", 0);
	shader.setInt("normalMap", 1);

	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);

	while (!glfwWindowShouldClose(window)) {
		//RENDER


		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		glClearColor(0.6f,0.8f,0.92f,1.0f);

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		shader.use();

		shader.setMat4("_Model", glm::mat4(1.0f));
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setVec3("_EyePos", camera.position);

		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);

		shader.setFloat("rimCut", rimCut);
		shader.setFloat("rimThres", rimThres);
		//shader.setMat4("_Model", monkeyTransform.modelMatrix());
		//monkeyModel.draw(); //Draws monkey model using current shader
		glBindTextureUnit(0, sandColor);
		glBindTextureUnit(1, sandNormalGL);
		shader.setMat4("_Model", sandTransform.modelMatrix());
		monkeyModel.draw();

		glBindTextureUnit(0, christmasColor);
		glBindTextureUnit(1, christmasNormalGL);
		shader.setMat4("_Model", christmasTransform.modelMatrix());
		bunnyModel.draw();

		glBindTextureUnit(0, porceColor);
		glBindTextureUnit(1, porceNormalGL);
		shader.setMat4("_Model", porceTransform.modelMatrix());
		monkeyModel.draw();

		glBindTextureUnit(0, rockColor);
		glBindTextureUnit(1, rockNormalGL);
		shader.setMat4("_Model", rockTransform.modelMatrix());
		monkeyModel.draw();

		glBindTextureUnit(0, concreteColor);
		glBindTextureUnit(1, concreteNormalGL);
		shader.setMat4("_Model", concreteTransform.modelMatrix());
		monkeyModel.draw();
		
		glBindTextureUnit(0, tileColor);
		glBindTextureUnit(1, tileNormalGL);
		shader.setMat4("_Model", tileTransform.modelMatrix());
		monkeyModel.draw();

		cameraController.move(window, &camera, deltaTime);
		//Spin all the models
		christmasTransform.rotation = glm::rotate(christmasTransform.rotation, deltaTime, glm::vec3(0.0, 0.5, 0.0));
		sandTransform.rotation = glm::rotate(sandTransform.rotation, deltaTime, glm::vec3(0.1, 0.0, 0.0));
		tileTransform.position = glm::vec3(tileTransform.position.x, glm::sin(time * 2) * 2, 0.0f);
		concreteTransform.position = glm::vec3(concreteTransform.position.x, glm::cos(time * 2) * 0.5, 0.0f);
		//porceTransform.position = glm::vec3(glm::sin(time * 2) * 50.0f, 200.0f, 0.0f);
		rockTransform.position = glm::vec3(glm::sin(time * 2) * 0.5f, -1.0f, 0.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		outlineShader.use();
		outlineShader.setVec3("EdgeColor", edgeColor);
		outlineShader.setFloat("outlineX", outlineX);
		outlineShader.setFloat("outlineY", outlineY);
		outlineShader.setFloat("offset", offset);
	
		glBindTextureUnit(0, framebuffer.colorBuffer[0]);
		glBindVertexArray(dummyVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffer[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);

	}

	if (ImGui::CollapsingHeader("Toon Shading"))
	{
		ImGui::SliderFloat("Rim Threshold", &rimThres, 0.0f, 15.0f);
		ImGui::SliderFloat("Rim CutOff", &rimCut, -1.5f, 1.5f);
	}

	if (ImGui::CollapsingHeader("Sobel Edge"))
	{
		ImGui::ColorEdit3("Edge Color", &edgeColor.r);
		ImGui::SliderFloat("Outline Size X", &outlineX, 0.0f, 1.0f);
		ImGui::SliderFloat("Outline Size Y", &outlineY, 0.0f, 1.0f);
		ImGui::SliderFloat("Offset", &offset, -5.0f, 5.0f);
	}

	if (ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

