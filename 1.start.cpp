#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <sphere1.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouseclick_callback(GLFWwindow* window, int button, int action, int mods);

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

vector<float> makecircle(float radius, int count);

using namespace std;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float swt = 1;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


int main() {
	//-------------------------------�������-------------------------------------
	glfwInit(); //glfw�ʱ�ȭ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//(�츮�� ������ �ɼ�, �ɼǰ�)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); //(����, �ʺ�,â�̸�, null, null) ��ü����

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); //window�� ��context�� ����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //â�� ũ�Ⱑ ����ɶ����� �Լ��� ȣ���ϵ��� ���

	//GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//glad�� �ʱ�ȭ
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//--------------------------������� ���� (�⺻����)------------------------------------

	glActiveTexture(GL_TEXTURE0); //�ؽ�ó�� ���ε��ϱ� ���� �ؽ�ó ������ Ȱ��ȭ
	unsigned int texture1 = loadTexture("photo/container2.png");

	glActiveTexture(GL_TEXTURE1); //�ؽ�ó�� ���ε��ϱ� ���� �ؽ�ó ������ Ȱ��ȭ
	unsigned int texture2 = loadTexture("photo/container2_specular.png");

	glActiveTexture(GL_TEXTURE0);
	unsigned int texture3 = loadTexture("photo/tile.jpg");


	value vec;
	vec = sphere(1, 40);
	vector<float> ver = vec.vertices;
	vector<int> ind = vec.indices;

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
	};

	float flat[] = {
	 10.0f, -0.51f, -10.0f, 10.0f, 10.0f,
	-10.0f, -0.51f, -10.0f, 0.0f, 10.0f,
	-10.0f, -0.51f,  10.0f, 0.0f, 0.0f,
	-10.0f, -0.51f,  10.0f, 0.0f, 0.0f,
	 10.0f, -0.51f,  10.0f, 10.0f, 0.0f,
	 10.0f, -0.51f, -10.0f, 10.0f, 10.0f
	};

	unsigned int sphereVbo;
	unsigned int sphereEbo;
	unsigned int sphereVao;

	glGenBuffers(1, &sphereVbo);
	glGenBuffers(1, &sphereEbo);
	glGenVertexArrays(1, &sphereVao);

	glBindVertexArray(sphereVao);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
	glBufferData(GL_ARRAY_BUFFER, ver.size() * sizeof(float), &ver[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(float), &ind[0], GL_STATIC_DRAW);






	unsigned int VBO[2];//vertex buffer object
	unsigned int lightVAO;

	glGenBuffers(2, &VBO[0]); //���� id�� ����
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO); //array ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);//������ ������ �����Ͽ� ���ε�
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//�����͸� ������ �޸𸮿� ����

	//opengl���� vertex�����͸� ��� �ؼ��ؾ� �ϴ����� �˷���
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);//������ ������ �����Ͽ� ���ε�
	glBufferData(GL_ARRAY_BUFFER, sizeof(flat), flat, GL_STATIC_DRAW);//�����͸� ������ �޸𸮿� ����

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(4);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouseclick_callback);


	Shader modelload("shader/modelload.vs", "shader/modelload.fs");// �갡 shader�� ���� ������ ��ũ���� ������
	Shader floor("shader/floor.vs", "shader/floor.fs");
	Shader lamp("shader/lamp.vs", "shader/lamp.fs");
	Shader light("shader/multi_light.vs", "shader/multi_light.fs");
	Shader screenShader("shader/screen.vs", "shader/screen.fs");
	Shader skybox("shader/skybox.vs", "shader/skybox.fs");
	Shader reflect("shader/reflect.vs", "shader/reflect.fs");
	Shader drsp("shader/sphere.vs", "shader/sphere.fs");



	light.use();
	glUniform1i(glGetUniformLocation(light.ID, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(light.ID, "material.specular"), 1);


	//material shininess �Է�
	float shininessmaterial = 64;
	glUniform1f(glGetUniformLocation(light.ID, "material.shininess"), shininessmaterial);

	//directional light�� ������ �Է�
	glm::vec3 directlightColor(1.0f);
	glm::vec3 directionlight(-0.2f, -1.0f, -0.3f);

	glm::vec3 diffuselight = directlightColor * glm::vec3(0.5f);
	glm::vec3 ambientlight = directlightColor * glm::vec3(0.1f);
	glm::vec3 specularlight = directlightColor * glm::vec3(1.0f);

	glUniform3fv(glGetUniformLocation(light.ID, "direct.ambient"), 1, &ambientlight[0]);
	glUniform3fv(glGetUniformLocation(light.ID, "direct.diffuse"), 1, &diffuselight[0]);
	glUniform3fv(glGetUniformLocation(light.ID, "direct.specular"), 1, &specularlight[0]);
	glUniform3fv(glGetUniformLocation(light.ID, "direct.direction"), 1, &directionlight[0]);


	//point light�� ������ �Է�
	glm::vec3 pointlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	diffuselight = pointlightColor * glm::vec3(0.5f);
	ambientlight = pointlightColor * glm::vec3(0.0f);
	specularlight = pointlightColor * glm::vec3(1.0f);
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	glm::vec3 pointlightPos[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	for (int i = 0; i < 1; i++) {
		string p;
		p = "point[" + to_string(i) + "]";

		glUniform3fv(glGetUniformLocation(light.ID, (p + ".ambient").c_str()), 1, &ambientlight[0]);
		glUniform3fv(glGetUniformLocation(light.ID, (p + ".diffuse").c_str()), 1, &diffuselight[0]);
		glUniform3fv(glGetUniformLocation(light.ID, (p + ".specular").c_str()), 1, &specularlight[0]);
		glUniform3fv(glGetUniformLocation(light.ID, (p + ".position").c_str()), 1, &pointlightPos[i][0]);
		glUniform1f(glGetUniformLocation(light.ID, (p + ".constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(light.ID, (p + ".linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(light.ID, (p + ".quadratic").c_str()), quadratic);
	}




	floor.use();
	glUniform1i(glGetUniformLocation(floor.ID, "material2.diffuse"), 0);

	//material shininess �Է�
	glUniform1f(glGetUniformLocation(floor.ID, "material2.shininess"), shininessmaterial);

	//directional light�� ������ �Է�
	glUniform3fv(glGetUniformLocation(floor.ID, "direct2.ambient"), 1, &ambientlight[0]);
	glUniform3fv(glGetUniformLocation(floor.ID, "direct2.diffuse"), 1, &diffuselight[0]);
	glUniform3fv(glGetUniformLocation(floor.ID, "direct2.specular"), 1, &specularlight[0]);
	glUniform3fv(glGetUniformLocation(floor.ID, "direct2.direction"), 1, &directionlight[0]);

	//point light�� ������ �Է�
	for (int i = 0; i < 1; i++) {
		string p;
		p = "point2[" + to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(floor.ID, (p + ".ambient").c_str()), 1, &ambientlight[0]);
		glUniform3fv(glGetUniformLocation(floor.ID, (p + ".diffuse").c_str()), 1, &diffuselight[0]);
		glUniform3fv(glGetUniformLocation(floor.ID, (p + ".specular").c_str()), 1, &specularlight[0]);
		glUniform3fv(glGetUniformLocation(floor.ID, (p + ".position").c_str()), 1, &pointlightPos[i][0]);
		glUniform1f(glGetUniformLocation(floor.ID, (p + ".constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(floor.ID, (p + ".linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(floor.ID, (p + ".quadratic").c_str()), quadratic);
	}




	//render loop
	while (!glfwWindowShouldClose(window))//������ ���۵� ������ �����ϵ��� ���õǾ����� Ȯ��, �׷��ٸ� true��ȯ
	{
		float currentFrame = glfwGetTime(); //��ǻ���� ���μ��� �ӵ��� ���� ������ ī�޶� �̵��ӵ��� ���� �ϱ����� ��ġ
		deltaTime = currentFrame - lastFrame; //�� ������ ó���ϴµ� �ɸ��� �ð�
		lastFrame = currentFrame;

		processInput(window); //esc�� ���ȴ��� �������� Ȯ��


		glEnable(GL_DEPTH_TEST);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), (float)800 / 600, 0.1f, 100.0f);

		glm::mat4 model = glm::mat4(1.0f);






		//lamp----------------------------------------------------------------
		lamp.use();
		glUniformMatrix4fv(glGetUniformLocation(lamp.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lamp.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(glGetUniformLocation(lamp.ID, "lightcolor"), 1, &pointlightColor[0]);



		model = glm::mat4(1.0f);
		for (int i = 0; i < 1; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointlightPos[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lamp.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//-----------------------------------------------------------------

		//sphere--------------------------------------------------------------------
		drsp.use();
		glBindVertexArray(sphereVao);
		glUniformMatrix4fv(glGetUniformLocation(drsp.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(drsp.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -5.0f));
		glUniformMatrix4fv(glGetUniformLocation(drsp.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, NULL);


		//cube-------------------------------------------------------------------
		light.use(); //�� ���α׷� ��ü�� ����ϰڴٰ� ����
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(lightVAO);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform3fv(glGetUniformLocation(light.ID, "viewPos"), 1, &cameraPos[0]);

		//spot light�� ������ �Է�//////////////
		glm::vec3 spotlightColor = glm::vec3(1.0f);
		diffuselight = spotlightColor * glm::vec3(1.5f) * swt;
		ambientlight = spotlightColor * glm::vec3(0.05f) * swt;
		specularlight = spotlightColor * glm::vec3(2.0f) * swt;
		directionlight = cameraFront;
		glm::vec3 spotlightPos = cameraPos;

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;
		float cutoff = glm::cos(glm::radians(8.5f));
		float outercutoff = glm::cos(glm::radians(13.5f));

		glUniform3fv(glGetUniformLocation(light.ID, "spot.ambient"), 1, &ambientlight[0]);
		glUniform3fv(glGetUniformLocation(light.ID, "spot.diffuse"), 1, &diffuselight[0]);
		glUniform3fv(glGetUniformLocation(light.ID, "spot.specular"), 1, &specularlight[0]);

		glUniform3fv(glGetUniformLocation(light.ID, "spot.direction"), 1, &directionlight[0]);
		glUniform3fv(glGetUniformLocation(light.ID, "spot.position"), 1, &spotlightPos[0]);
		glUniform1f(glGetUniformLocation(light.ID, "spot.cutoff"), cutoff);
		glUniform1f(glGetUniformLocation(light.ID, "spot.outercutoff"), outercutoff);

		glUniform1f(glGetUniformLocation(light.ID, "spot.constant"), constant);
		glUniform1f(glGetUniformLocation(light.ID, "spot.linear"), linear);
		glUniform1f(glGetUniformLocation(light.ID, "spot.quadratic"), quadratic);
		/////////////////////////////////////////////////

		glUniformMatrix4fv(glGetUniformLocation(light.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(light.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.6f, 1.5f, 0.6f));

		glUniformMatrix4fv(glGetUniformLocation(light.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36); //�׸���

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.4f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.7f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(light.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36); //�׸���

		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.4f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		model = glm::translate(model, glm::vec3(0.0f, -0.6f, 0.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.4f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));


		glUniformMatrix4fv(glGetUniformLocation(light.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36); //�׸���


		//---------------------------------------------------------------------------

		//floor-------------------------------------------------------------------
		floor.use(); //�� ���α׷� ��ü�� ����ϰڴٰ� ����
		glBindTexture(GL_TEXTURE_2D, texture3);
		glUniform3fv(glGetUniformLocation(floor.ID, "viewPos"), 1, &cameraPos[0]);

		//spot light�� ������ �Է�//////////////
		spotlightColor = glm::vec3(1.0f);
		diffuselight = spotlightColor * glm::vec3(1.5f) * swt;
		ambientlight = spotlightColor * glm::vec3(0.5f) * swt;
		specularlight = spotlightColor * glm::vec3(2.0f) * swt;
		directionlight = cameraFront;
		spotlightPos = cameraPos;

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;
		cutoff = glm::cos(glm::radians(8.5f));
		outercutoff = glm::cos(glm::radians(13.5f));

		glUniform3fv(glGetUniformLocation(floor.ID, "spot2.ambient"), 1, &ambientlight[0]);
		glUniform3fv(glGetUniformLocation(floor.ID, "spot2.diffuse"), 1, &diffuselight[0]);
		glUniform3fv(glGetUniformLocation(floor.ID, "spot2.specular"), 1, &specularlight[0]);

		glUniform3fv(glGetUniformLocation(floor.ID, "spot2.direction"), 1, &directionlight[0]);
		glUniform3fv(glGetUniformLocation(floor.ID, "spot2.position"), 1, &spotlightPos[0]);
		glUniform1f(glGetUniformLocation(floor.ID, "spot2.cutoff"), cutoff);
		glUniform1f(glGetUniformLocation(floor.ID, "spot2.outercutoff"), outercutoff);

		glUniform1f(glGetUniformLocation(floor.ID, "spot2.constant"), constant);
		glUniform1f(glGetUniformLocation(floor.ID, "spot2.linear"), linear);
		glUniform1f(glGetUniformLocation(floor.ID, "spot2.quadratic"), quadratic);
		/////////////////////////////////////////////////

		glUniformMatrix4fv(glGetUniformLocation(floor.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(floor.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.0f));
		glUniformMatrix4fv(glGetUniformLocation(floor.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glDrawArrays(GL_TRIANGLES, 0, 6); //�׸���
		//---------------------------------------------------------------------------

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO[0]);
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) //â�� ũ�⸦ �����ϴ� �Լ� (��ü, �ʺ�, ����)
{
	glViewport(0, 0, width, height); //�������� ���� �Ʒ� �𼭸��� ��ġ, �ʺ�� ������ �ȼ� (-1~1 ������ ��ǥ���� ���ϴ� ���� �ȼ������Ϸ� ��ȯ)
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == 1)
		glfwSetWindowShouldClose(window, true);
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	//cameraPos.y = 0;
}

void mouseclick_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		swt = -swt + 1;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = (front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		faces[i] = "photo/skybox2/" + faces[i];
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

vector<float> makecircle(float radius, int count) {
	vector<float> circle;
	float theta;
	for (int i = 0; i < count; i++) {
		theta = 2 * 3.141592 * i / count;
		circle.push_back(radius * cos(theta));
		circle.push_back(radius * sin(theta));
		circle.push_back(0);
	}

	return circle;
}