#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <shader.h>
#include <sphere1.h>
#include <spline.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

bool compare(pair<float, pair<glm::vec3, glm::mat4>> a, pair<float, pair<glm::vec3, glm::mat4>> b);
void drawdrone(unsigned int dbvao, unsigned int clvao, unsigned int ppvao, unsigned int dlvao, int dbindices, int clindices, int ppindices, int dlindices, bool Shadow, Shader spline);
void drawcube(unsigned int squarevao, Shader spline);
int drawtree(unsigned int clvao, int clindices, glm::mat4 treemodel, int repeatnum, Shader spline);

using namespace std;
float swt = 1;
float fov = 45.0f;

bool droneview = false;

glm::vec3 cameraPos = glm::vec3(0.0f, 100.0f, 220.0f);
glm::vec3 dronefront = glm::vec3(1.0f, 0.0f, -1.0f);

glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 centerofrot(0.0f, 0.0f, 0.0f);

glm::vec3 dronepos = glm::vec3(-50, 0, 50);

glm::vec3 posvec;
glm::vec3 rotate_axis;
float rotate_theta;



int main() {
	//-------------------------------여기부터-------------------------------------
	glfwInit(); //glfw초기화
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//(우리가 설정할 옵션, 옵션값)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); //(높이, 너비,창이름, null, null) 객체생성

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); //window를 주context로 지정
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //창의 크기가 변경될때마다 함수를 호출하도록 등록

	//GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//glad를 초기화
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//--------------------------여기까지 no touch (기본설정)--------------------------

	float square[] = {
-1.0f, 0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
 1.0f, 0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
-1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
-1.0f, 0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	};

	//square--------------------------------------
	unsigned int squarevao, squarevbo;
	glGenBuffers(1, &squarevbo); //버퍼 id를 생성
	glGenVertexArrays(1, &squarevao);

	glBindVertexArray(squarevao); //array 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, squarevbo);//버퍼의 유형을 선택하여 바인딩
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);//데이터를 버퍼의 메모리에 저장

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	//vertex 좌표
	//texture attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	//normal vector 좌표


	//sphere------------------------------------------------------
	value vec;
	vec = sphere(1, 40);
	vector<float> ver = vec.vertices;
	vector<int> ind = vec.indices;
	vector<int> ind_grid = vec.indices_grid;

	unsigned int sphereVbo;
	unsigned int sphereEbo, sphereEbo2;
	unsigned int sphereVao, sphereVao_grid;
	//색칠-----------------------
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

	//grid-----------------
	glGenBuffers(1, &sphereEbo2);
	glGenVertexArrays(1, &sphereVao_grid);
	glBindVertexArray(sphereVao_grid);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
	glBufferData(GL_ARRAY_BUFFER, ver.size() * sizeof(float), &ver[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEbo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_grid.size() * sizeof(float), &ind_grid[0], GL_STATIC_DRAW);


	//temp-----------------------------------------------------------------------------------------------------------------------
	int ct = 30; //단면을 구성하는 control point 개수
	int step1 = 30; //단면쪼개는횟수
	int step2 = 5; //swept쪼개는 횟수

	int clockwise = 1;//단면의 좌표들이 배치된 방향 (시계방향이면 -1)


	sweptsurface tempObject("coke_bottle.txt", ct, step2, step1, clockwise);
	vector<float> tocoord = tempObject.sweptcoord;
	vector<float> tonormal = tempObject.normalcoord;
	vector<int> toindices = tempObject.paintindices;
	unsigned int tovbo, tovbo2, tovao, toebo;

	//color
	glGenBuffers(1, &tovbo);
	glGenBuffers(1, &tovbo2);
	glGenBuffers(1, &toebo);
	glGenVertexArrays(1, &tovao);

	glBindVertexArray(tovao);
	glBindBuffer(GL_ARRAY_BUFFER, tovbo);
	glBufferData(GL_ARRAY_BUFFER, tocoord.size() * sizeof(float), &tocoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, tovbo2);
	glBufferData(GL_ARRAY_BUFFER, tonormal.size() * sizeof(float), &tonormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, toebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, toindices.size() * sizeof(float), &toindices[0], GL_STATIC_DRAW);


	//-------------------------------------------------------------------------------------------------------------------------

	/*
	//propeller-----------------------------------------------------------------------------------------------------------------------
	int ct = 4; //단면을 구성하는 control point 개수
	int step1 = 30; //단면쪼개는횟수
	int step2 = 5; //swept쪼개는 횟수

	int clockwise = 1;//단면의 좌표들이 배치된 방향 (시계방향이면 -1)


	sweptsurface propeller("propeller.txt", ct, step2, step1, clockwise);
	vector<float> ppcoord = propeller.sweptcoord;
	vector<float> ppnormal = propeller.normalcoord;
	vector<int> ppindices = propeller.paintindices;
	unsigned int ppvbo, ppvbo2, ppvao, ppebo;

	//color
	glGenBuffers(1, &ppvbo);
	glGenBuffers(1, &ppvbo2);
	glGenBuffers(1, &ppebo);
	glGenVertexArrays(1, &ppvao);

	glBindVertexArray(ppvao);
	glBindBuffer(GL_ARRAY_BUFFER, ppvbo);
	glBufferData(GL_ARRAY_BUFFER, ppcoord.size() * sizeof(float), &ppcoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ppvbo2);
	glBufferData(GL_ARRAY_BUFFER, ppnormal.size() * sizeof(float), &ppnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ppebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ppindices.size() * sizeof(float), &ppindices[0], GL_STATIC_DRAW);


	//-------------------------------------------------------------------------------------------------------------------------
	*/

	//dronebody----------------------------------------------------------------------------------------------------------------
	ct = 12; //단면을 구성하는 control point 개수
	step1 = 60; //단면쪼개는횟수
	step2 = 5; //swept쪼개는 횟수

	clockwise = 1;//단면의 좌표들이 배치된 방향 (시계방향이면 -1)


	sweptsurface dronebody("dronebody.txt", ct, step2, step1, clockwise);
	vector<float> dbcoord = dronebody.sweptcoord;
	vector<float> dbnormal = dronebody.normalcoord;
	vector<int> dbindices = dronebody.paintindices;

	/*
	for (int i = 0; i < dbcoord.size(); i++) {

		if(i%3==1) cout << dbcoord[i] << endl;
	}
	*/

	unsigned int dbvbo, dbvbo2, dbvao, dbebo;


	glGenVertexArrays(1, &dbvao);
	glBindVertexArray(dbvao);

	glGenBuffers(1, &dbvbo);
	glGenBuffers(1, &dbvbo2);
	glGenBuffers(1, &dbebo);

	glBindBuffer(GL_ARRAY_BUFFER, dbvbo);
	glBufferData(GL_ARRAY_BUFFER, dbcoord.size() * sizeof(float), &dbcoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, dbvbo2);
	glBufferData(GL_ARRAY_BUFFER, dbnormal.size() * sizeof(float), &dbnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dbebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dbindices.size() * sizeof(float), &dbindices[0], GL_STATIC_DRAW);

	//--------------------------------------------------------------------------------------------------------------------

	//cylinder----------------------------------------------------------------------------------------------------------------
	ct = 4; //단면을 구성하는 control point 개수
	step1 = 30; //단면쪼개는횟수
	step2 = 5; //swept쪼개는 횟수

	clockwise = 1;//단면의 좌표들이 배치된 방향 (시계방향이면 -1)


	sweptsurface cylinder("cylinder.txt", ct, step2, step1, clockwise);
	vector<float> clcoord = cylinder.sweptcoord;
	vector<float> clnormal = cylinder.normalcoord;
	vector<int> clindices = cylinder.paintindices;

	/*
	for (int i = 0; i < dbcoord.size(); i++) {

		if(i%3==1) cout << dbcoord[i] << endl;
	}
	*/

	unsigned int clvbo, clvbo2, clvao, clebo;


	glGenVertexArrays(1, &clvao);
	glBindVertexArray(clvao);

	glGenBuffers(1, &clvbo);
	glGenBuffers(1, &clvbo2);
	glGenBuffers(1, &clebo);

	glBindBuffer(GL_ARRAY_BUFFER, clvbo);
	glBufferData(GL_ARRAY_BUFFER, clcoord.size() * sizeof(float), &clcoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, clvbo2);
	glBufferData(GL_ARRAY_BUFFER, clnormal.size() * sizeof(float), &clnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, clindices.size() * sizeof(float), &clindices[0], GL_STATIC_DRAW);

	//--------------------------------------------------------------------------------------------------------------------

	//droneleg----------------------------------------------------------------------------------------------------------------
	ct = 4; //단면을 구성하는 control point 개수
	step1 = 30; //단면쪼개는횟수
	step2 = 5; //swept쪼개는 횟수

	clockwise = 1;//단면의 좌표들이 배치된 방향 (시계방향이면 -1)


	sweptsurface droneleg("droneleg.txt", ct, step2, step1, clockwise);
	vector<float> dlcoord = droneleg.sweptcoord;
	vector<float> dlnormal = droneleg.normalcoord;
	vector<int> dlindices = droneleg.paintindices;


	unsigned int dlvbo, dlvbo2, dlvao, dlebo;


	glGenVertexArrays(1, &dlvao);
	glBindVertexArray(dlvao);

	glGenBuffers(1, &dlvbo);
	glGenBuffers(1, &dlvbo2);
	glGenBuffers(1, &dlebo);

	glBindBuffer(GL_ARRAY_BUFFER, dlvbo);
	glBufferData(GL_ARRAY_BUFFER, dlcoord.size() * sizeof(float), &dlcoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, dlvbo2);
	glBufferData(GL_ARRAY_BUFFER, dlnormal.size() * sizeof(float), &dlnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dlebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dlindices.size() * sizeof(float), &dlindices[0], GL_STATIC_DRAW);

	//--------------------------------------------------------------------------------------------------------------------

	//cave----------------------------------------------------------------------------------------------------------------
	ct = 6; //단면을 구성하는 control point 개수
	step1 = 50; //단면쪼개는횟수
	step2 = 5; //swept쪼개는 횟수

	clockwise = 1;//단면의 좌표들이 배치된 방향 (시계방향이면 -1)


	sweptsurface cave("cave.txt", ct, step2, step1, clockwise);
	vector<float> cvcoord = cave.sweptcoord;
	vector<float> cvnormal = cave.normalcoord;
	vector<int> cvindices = cave.paintindices;


	unsigned int cvvbo, cvvbo2, cvvao, cvebo;


	glGenVertexArrays(1, &cvvao);
	glBindVertexArray(cvvao);

	glGenBuffers(1, &cvvbo);
	glGenBuffers(1, &cvvbo2);
	glGenBuffers(1, &cvebo);

	glBindBuffer(GL_ARRAY_BUFFER, cvvbo);
	glBufferData(GL_ARRAY_BUFFER, cvcoord.size() * sizeof(float), &cvcoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, cvvbo2);
	glBufferData(GL_ARRAY_BUFFER, cvnormal.size() * sizeof(float), &cvnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cvebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cvindices.size() * sizeof(float), &cvindices[0], GL_STATIC_DRAW);

	//--------------------------------------------------------------------------------------------------------------------


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);


	Shader spline("shader/spline.vs", "shader/spline.fs");

	spline.use();

	//material information
	glm::vec3 diffusecolor;
	glm::vec3 specularcolor;
	float alpha;
	float shininess;

	//light information
	glm::vec3 lightcolor(1.0f);

	glm::vec3 diffuselight;
	glm::vec3 ambientlight;
	glm::vec3 specularlight;

	//direct light
	glm::vec3 directlightcolor = glm::vec3(0.0f);
	glm::vec3 direction(-1.0f, -1.0f, 0.0f);
	diffuselight = directlightcolor * glm::vec3(0.8f);
	ambientlight = directlightcolor * glm::vec3(0.2f);
	specularlight = directlightcolor * glm::vec3(0.7f);

	glUniform3fv(glGetUniformLocation(spline.ID, "direct.ambient"), 1, &ambientlight[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "direct.diffuse"), 1, &diffuselight[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "direct.specular"), 1, &specularlight[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "direct.direction"), 1, &direction[0]);


	//point light

	glm::vec3 pointlightcolor[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};

	glm::vec3 diffuselightcolor[] = {
	 pointlightcolor[0] * glm::vec3(0.8f),
	 pointlightcolor[1] * glm::vec3(0.8f),
	 pointlightcolor[2] * glm::vec3(0.8f)
	};

	glm::vec3 ambientlightcolor[] = {
	 pointlightcolor[0] * glm::vec3(0.3f),
	 pointlightcolor[1] * glm::vec3(0.3f),
	 pointlightcolor[2] * glm::vec3(0.0f)
	};

	glm::vec3 specularlightcolor[] = {
	 pointlightcolor[0] * glm::vec3(0.7f),
	 pointlightcolor[1] * glm::vec3(0.7f),
	 pointlightcolor[2] * glm::vec3(0.7f)
	};

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	glm::vec3 pointlightPos[] = {
		glm::vec3(-90.0f,  100.0f,  -50.0f),
		glm::vec3(90.0f, 70.0f, 50.0f),
		glm::vec3(20.0f, 30.0f, -80.0f)
	};

	for (int i = 0; i < 3; i++) {
		string p;
		p = "point[" + to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(spline.ID, (p + ".ambient").c_str()), 1, &ambientlightcolor[i][0]);
		glUniform3fv(glGetUniformLocation(spline.ID, (p + ".diffuse").c_str()), 1, &diffuselightcolor[i][0]);
		glUniform3fv(glGetUniformLocation(spline.ID, (p + ".specular").c_str()), 1, &specularlightcolor[i][0]);
		glUniform3fv(glGetUniformLocation(spline.ID, (p + ".position").c_str()), 1, &pointlightPos[i][0]);
		glUniform1f(glGetUniformLocation(spline.ID, (p + ".constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(spline.ID, (p + ".linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(spline.ID, (p + ".quadratic").c_str()), quadratic);
	}

	//-------------------------------------------------------------------------


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//render loop
	while (!glfwWindowShouldClose(window))//루프가 시작될 때마다 종료하도록 지시되었는지 확인, 그렇다면 true반환
	{
		float currenttime = glfwGetTime(); 
		processInput(window); //esc가 눌렸는지 루프마다 확인

		glClearColor(0.6f, 0.85f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		view = glm::lookAt(cameraPos, centerofrot, cameraUp);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), (float)800 / 600, 0.1f, 500.0f);

		glm::mat4 model = glm::mat4(1.0f);
		
		//--------------------------------------------------------
		spline.use();
		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(glGetUniformLocation(spline.ID, "viewPos"), 1, &cameraPos[0]);
		glUniform1f(glGetUniformLocation(spline.ID, "shadow"), 1.0f);


		//sphere--------------------------------------------------------------------
		//material information
		glm::vec3 diffusecolor = glm::vec3(10.0f, 10.0f, 10.0f);
		glm::vec3 specularcolor = glm::vec3(0.0f, 0.0f, 0.0f);
		alpha = 1;
		shininess = 64;
		glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

		for (glm::vec3 a : pointlightPos) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, a);
			model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
			glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(sphereVao);
			glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, NULL);
		}

		//-------------------------------------------------------------------------
		//drawdrone(dbvao, clvao, ppvao, dlvao, dbindices.size(), clindices.size(), ppindices.size(), dlindices.size(), true, spline);//shadow
		//drawdrone(dbvao, clvao, ppvao, dlvao, dbindices.size(), clindices.size(), ppindices.size(), dlindices.size(), false, spline);

		//cave--------------------------------------------------------------------
		//material information
		diffusecolor = glm::vec3(1.0f, 1.0f, 1.0f);
		specularcolor = glm::vec3(0.0f, 0.0f, 0.0f);
		alpha = 1;
		shininess = 64;
		glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

		//matrix
		glm::mat4 cave = glm::mat4(1.0f);
		cave = glm::translate(cave, glm::vec3(0,-17,0));
		cave = glm::scale(cave, glm::vec3(2.0f));

		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(cave));
		glBindVertexArray(cvvao);
		//glDrawElements(GL_TRIANGLES, cvindices.size(), GL_UNSIGNED_INT, NULL);


		/*
		//temp--------------------------------------------------------------------
		//material information
		diffusecolor = glm::vec3(1.0f, 1.0f, 1.0f);
		specularcolor = glm::vec3(1.0f, 1.0f, 1.0f);
		alpha = 1;
		shininess = 64;
		glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

		//matrix
		glm::mat4 trombone = glm::mat4(1.0f);
		trombone = glm::translate(trombone, glm::vec3(0, -10, 0));
		trombone = glm::scale(trombone, glm::vec3(1.0f));

		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(trombone));
		glBindVertexArray(tovao);
		glDrawElements(GL_TRIANGLES, toindices.size(), GL_UNSIGNED_INT, NULL);
		*/


		/*
		//plane---------------------------------------------------------------------------
		diffusecolor = glm::vec3(1.3f, 1.1f, 0.8f);
		specularcolor = glm::vec3(0.0f, 0.0f, 0.0f);
		alpha = 1;
		shininess = 64;
		glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

		glBindVertexArray(squarevao);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -10.1f, 0.0f));
		model = glm::scale(model, glm::vec3(150.0f));
		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//tree--------------------------------------------------------------------
		diffusecolor = glm::vec3(0.69f, 0.57f, 0.48f);
		specularcolor = glm::vec3(0.1f, 0.1f, 0.1f);
		float alpha = 1;
		float shininess = 64;
		glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
		glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);
		glUniform3fv(glGetUniformLocation(spline.ID, "viewPos"), 1, &cameraPos[0]);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-80, -26, 0));
		glBindVertexArray(clvao);

		drawtree(clvao, clindices.size(), model, 6, spline);
		model = glm::translate(model, glm::vec3(0, 16, 0));
		model = glm::scale(model, glm::vec3(1.8, 4,1.8));
		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, clindices.size(), GL_UNSIGNED_INT, NULL);

		//cube-------------------------------------------------------------------------------------------
		*/
		drawcube(squarevao, spline);
		//---------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}





void framebuffer_size_callback(GLFWwindow* window, int width, int height) //창의 크기를 조정하는 함수 (객체, 너비, 높이)
{
	glViewport(0, 0, width, height); //윈도우의 왼쪽 아래 모서리의 위치, 너비와 높이의 픽셀 (-1~1 사이의 좌표값을 원하는 값의 픽셀스케일로 변환)
}


void mouseclick_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		swt = -swt + 1;
}

int temp = 1;
glm::vec3 firstvec;
glm::vec3 secondvec;
glm::vec3 xvec(1, 0, 0);
glm::vec3 yvec(0, 1, 0);
glm::quat rotate_quat(1,0,0,0);
glm::vec3 firstcamera;

glm::vec3 posvec2;
float lastX, lastY;
bool firstmouse = true;

float yaw = -90.0f;
float pitch = 0.0f;

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!droneview) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			posvec.x = (xpos - 400);
			posvec.y = (300 - ypos);
			posvec.z = 0;

			if (temp == 1) {
				xvec = glm::normalize(cross(glm::vec3(0, 1, 0), (cameraPos - centerofrot)));
				yvec = glm::normalize(cross(cameraPos - centerofrot, xvec));
				firstvec = cameraPos - centerofrot + xvec * posvec.x + yvec * posvec.y;
				temp = 0;
			}

			secondvec = cameraPos - centerofrot + xvec * posvec.x + yvec * posvec.y;

			if (firstvec != secondvec) {
				glm::vec3 cross = glm::cross(secondvec, firstvec);
				rotate_axis = glm::normalize(cross);
				rotate_theta = atan2(glm::length(cross), glm::dot(firstvec, secondvec));
				rotate_quat = glm::angleAxis(rotate_theta, rotate_axis);
				cameraPos = rotate_quat * (cameraPos - centerofrot) * glm::inverse(rotate_quat) + centerofrot;
				//xvec = rotate_quat * xvec * glm::inverse(rotate_quat);
				//yvec = rotate_quat * yvec * glm::inverse(rotate_quat);
				//cameraUp = yvec;
				temp = 1;
			}
		}

		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			temp = 1;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			if (firstmouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstmouse = false;
				firstvec = cameraPos - centerofrot;
			}
			float xoffset = -(xpos - lastX) * glm::length(cameraPos) * 0.0013;
			float yoffset = -(lastY - ypos) * glm::length(cameraPos) * 0.0013;
			lastX = xpos;
			lastY = ypos;

			posvec2 = xoffset * xvec + yoffset * yvec;
			//cout <<(cameraPos.z-centerofrot.z)<<endl;
			cameraPos += posvec2;
			centerofrot += posvec2;
		}

		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			firstmouse = true;
		}
	}
}


void processInput(GLFWwindow* window)
{
	glm::vec3 axis(0,1,0);
	float theta = 0.02f;
	glm::quat quat = glm::angleAxis(theta, axis);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == 1)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 1.0f * 0.02f;
	if (!droneview) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //dolly in
			cameraPos -= cameraSpeed * (cameraPos - centerofrot);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //dolly out
			cameraPos += cameraSpeed * (cameraPos - centerofrot);
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			dronepos += cameraSpeed * 30.0f * dronefront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			dronepos -= cameraSpeed * 30.0f * dronefront;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
			dronefront = glm::inverse(quat) * dronefront * quat;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
			dronefront = quat * dronefront * glm::inverse(quat);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			dronepos += cameraSpeed * 30.0f * glm::vec3(0,1,0);
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			dronepos -= cameraSpeed * 30.0f * glm::vec3(0, 1, 0);
			
		cameraPos = dronepos - dronefront * 15.0f + glm::vec3(0.0f, 7.0f, 0.0f);
		centerofrot = cameraPos + dronefront;
	}
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		if (!droneview) {
			cameraPos = dronepos  - dronefront*15.0f + glm::vec3(0.0f, 7.0f, 0.0f);
			centerofrot = cameraPos + dronefront;
		}
		else {
			cameraPos = glm::vec3(0.0f, 50.0f, 170.0f);
			centerofrot = glm::vec3(0.0f);
		}
		droneview = !droneview;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{


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

bool compare(pair<float,pair<glm::vec3, glm::mat4>> a, pair<float, pair<glm::vec3,glm::mat4>> b) {
	return a.first <b.first;
}


void drawdrone(unsigned int dbvao, unsigned int clvao, unsigned int ppvao, unsigned int dlvao, int dbindices, int clindices, int ppindices, int dlindices, bool Shadow, Shader spline) {
	float currenttime = glfwGetTime();

	//dronebody--------------------------------------------------------------------
	//material information
	glm::vec3 diffusecolor = glm::vec3(2.0f, 2.0f, 2.0f);
	glm::vec3 specularcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1;
	float shininess = 64;
	glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "viewPos"), 1, &cameraPos[0]);

	//matrix
	glm::mat4 d0 = glm::mat4(1.0f);
	float rotheta = atan2(glm::dot(glm::cross(glm::vec3(1, 0, -1), dronefront), glm::vec3(0, 1, 0)), glm::dot(glm::vec3(1, 0, -1), dronefront));
	d0 = glm::translate(d0, glm::vec3(dronepos.x +!droneview * 5* sin(currenttime), dronepos.y , dronepos.z + !droneview * 5 * sin(currenttime)));
	d0 = glm::rotate(d0, rotheta, glm::vec3(0, 1, 0));
	d0 = glm::rotate(d0, 3.1416f, glm::vec3(0, 1, 0));
	d0 = glm::scale(d0, glm::vec3(0.3f));


	if (Shadow) {
		glm::mat4 m(1.0f);
		glm::mat4 t = glm::translate(m, glm::vec3(90.0f, -100.0f, 50.0f));
		glm::mat4 shadow = glm::mat4(1, 0, 0, 0, 0, 1, 0, float(-1) / 110, 0, 0, 1, 0, 0, 0, 0, 0);
		d0 = glm::inverse(t) * shadow * t * d0;
		glUniform1f(glGetUniformLocation(spline.ID, "shadow"), 0.0f);
	}
	else {
		glUniform1f(glGetUniformLocation(spline.ID, "shadow"), 1.0f);
	}
	

	//db
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(d0));
	glBindVertexArray(dbvao);
	glDrawElements(GL_TRIANGLES, dbindices, GL_UNSIGNED_INT, NULL);

	//droneleg--------------------------------------------------------------------
	//material information
	diffusecolor = glm::vec3(2.0f, 2.0f, 2.0f);
	specularcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	alpha = 1;
	shininess = 64;
	glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

	glUniform3fv(glGetUniformLocation(spline.ID, "viewPos"), 1, &cameraPos[0]);

	//matrix
	//leg1
	glm::mat4 leg1 = d0;
	leg1 = glm::rotate(leg1, -0.7854f, glm::vec3(0, 1, 0));
	leg1 = glm::translate(leg1, glm::vec3(-20.0f, -5.0f, 10.0f));
	leg1 = glm::scale(leg1, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(leg1));
	glBindVertexArray(dlvao);
	glDrawElements(GL_TRIANGLES, dlindices, GL_UNSIGNED_INT, NULL);

	//leg2
	glm::mat4 leg2 = d0;
	leg2 = glm::rotate(leg2, -0.7854f, glm::vec3(0, 1, 0));
	leg2 = glm::translate(leg2, glm::vec3(20.0f, -5.0f, 10.0f));
	leg2 = glm::scale(leg2, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(leg2));
	glBindVertexArray(dlvao);
	glDrawElements(GL_TRIANGLES, dlindices, GL_UNSIGNED_INT, NULL);


	//cylinder--------------------------------------------------------------------
	//material information
	diffusecolor = glm::vec3(2.0f, 2.0f, 2.0f);
	specularcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	alpha = 1;
	shininess = 64;
	glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

	glUniform3fv(glGetUniformLocation(spline.ID, "viewPos"), 1, &cameraPos[0]);

	//matrix
	//c1
	glm::mat4 c1 = d0;
	c1 = glm::translate(c1, glm::vec3(-35.0f, 2.0f, 0.0f));
	c1 = glm::scale(c1, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(c1));
	glBindVertexArray(clvao);
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);

	//c2
	glm::mat4 c2 = d0;
	c2 = glm::translate(c2, glm::vec3(35.0f, 2.0f, 0.0f));
	c2 = glm::scale(c2, glm::vec3(1.0f, 0.8f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(c2));
	glBindVertexArray(clvao);
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);

	//c3
	glm::mat4 c3 = d0;
	c3 = glm::translate(c3, glm::vec3(0.0f, 2.0f, 35.0f));
	c3 = glm::scale(c3, glm::vec3(1.0f, 0.8f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(c3));
	glBindVertexArray(clvao);
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);

	//c4
	glm::mat4 c4 = d0;
	c4 = glm::translate(c4, glm::vec3(0.0f, 2.0f, -35.0f));
	c4 = glm::scale(c4, glm::vec3(1.0f, 0.8f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(c4));
	glBindVertexArray(clvao);
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);


	//propeller----------------------------------------------------------------
	//material information
	diffusecolor = glm::vec3(2.0f, 2.0f, 2.0f);
	specularcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	alpha = 1;
	shininess = 64;
	glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);

	glUniform3fv(glGetUniformLocation(spline.ID, "viewPos"), 1, &cameraPos[0]);
	//matrix

	glm::mat4 p0 = glm::mat4(1.0f);
	p0 = glm::translate(p0, glm::vec3(0.0f, 8.0f, 0.0f));
	p0 = glm::rotate(p0, currenttime * 20.0f, glm::vec3(0, 1, 0));
	p0 = glm::scale(p0, glm::vec3(4.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 0.5f, glm::vec3(1, 0, 0));
	model = glm::rotate(model, 1.5708f, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(0.01f));
	glm::mat4 p1 = p0 * model;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 0.5f, glm::vec3(-1, 0, 0));
	model = glm::rotate(model, 1.5708f, glm::vec3(0, 0, -1));
	model = glm::scale(model, glm::vec3(0.01f));
	glm::mat4 p2 = p0 * model;

	model = glm::mat4(1.0f);
	glm::mat4 init = glm::rotate(model, 0.8f, glm::vec3(0, 1, 0));

	//p1_1
	glm::mat4 p1_1 = c1 * p1;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p1_1));
	glBindVertexArray(ppvao);
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

	//p1_2;
	glm::mat4 p1_2 = c1 * p2;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p1_2));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

	//p2_1
	glm::mat4 p2_1 = c2 * init * p1;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p2_1));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

	//p2_2;
	glm::mat4 p2_2 = c2 * init * p2;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p2_2));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

	//p3_1
	glm::mat4 p3_1 = c3 * init * init * p1;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p3_1));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

	//p3_2;
	glm::mat4 p3_2 = c3 * init * init * p2;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p3_2));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);


	//p4_1
	glm::mat4 p4_1 = c4 * init * init * init * p1;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p4_1));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

	//p4_2;
	glm::mat4 p4_2 = c4 * init * init * init * p2;
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(p4_2));
	glDrawElements(GL_TRIANGLES, ppindices, GL_UNSIGNED_INT, NULL);

}


void drawcube(unsigned int squarevao, Shader spline) {

	vector<pair<float, pair<glm::vec3, glm::mat4>>>v;
	//cube model matrix------------------------------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(80.0f, 20.0f, 40.0f));
	model = glm::scale(model, glm::vec3(15.0f));
	glm::mat4 model1 = model;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 model2 = model1 * model;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 model3 = model1 * model;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 model4 = model1 * model;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 model5 = model1 * model;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 model6 = model1 * model;
	//------------------------------------------------------------------
	//cube----------------------------------------------------------------
	glm::vec3 diffusecolor = glm::vec3(1.0f, 0.5f, 0.5f);
	glm::vec3 specularcolor = glm::vec3(1.0f, 0.5f, 0.5f);
	float alpha = 0.5;
	float shininess = 64;
	glUniform1f(glGetUniformLocation(spline.ID, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(spline.ID, "material.alpha"), alpha);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &diffusecolor[0]);
	glUniform3fv(glGetUniformLocation(spline.ID, "material.specular"), 1, &specularcolor[0]);


	glBindVertexArray(squarevao);

	float normal = glm::dot(glm::vec3(0, 1, 0), glm::normalize(cameraPos - centerofrot));
	v.push_back(pair <float, pair<glm::vec3, glm::mat4>>(normal, make_pair(glm::vec3(1.0f, 0.5f, 0.5f), model1)));
	normal = glm::dot(glm::vec3(-1, 0, 0), glm::normalize(cameraPos - centerofrot));
	v.push_back(pair <float, pair<glm::vec3, glm::mat4>>(normal, make_pair(glm::vec3(1.0f, 1.0f, 0.5f), model2)));
	normal = glm::dot(glm::vec3(0, -1, 0), glm::normalize(cameraPos - centerofrot));
	v.push_back(pair <float, pair<glm::vec3, glm::mat4>>(normal, make_pair(glm::vec3(0.5f, 1.0f, 0.5f), model3)));
	normal = glm::dot(glm::vec3(1, 0, 0), glm::normalize(cameraPos - centerofrot));
	v.push_back(pair <float, pair<glm::vec3, glm::mat4>>(normal, make_pair(glm::vec3(0.5f, 1.0f, 1.0f), model4)));
	normal = glm::dot(glm::vec3(0, 0, 1), glm::normalize(cameraPos - centerofrot));
	v.push_back(pair <float, pair<glm::vec3, glm::mat4>>(normal, make_pair(glm::vec3(1.0f, 0.5f, 1.0f), model5)));
	normal = glm::dot(glm::vec3(0, 0, -1), glm::normalize(cameraPos - centerofrot));
	v.push_back(pair <float, pair<glm::vec3, glm::mat4>>(normal, make_pair(glm::vec3(0.5f, 0.5f, 1.0f), model6)));

	sort(v.begin(), v.end(), compare); //depth sorting using normal vector
	for (pair<float, pair<glm::vec3, glm::mat4>> number : v) {
		glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(number.second.second));
		glUniform3fv(glGetUniformLocation(spline.ID, "material.diffuse"), 1, &number.second.first[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	v.clear();

}


int drawtree(unsigned int clvao, int clindices, glm::mat4 treemodel , int repeatnum, Shader spline) {

	repeatnum -= 1;
	if (repeatnum < 0) {
		return 0;
	}
	//tree--------------------------------------------------------------------

	//matrix
	glm::mat4 d0 = treemodel;

	glm::mat4 i(1.0f);
	glm::mat4 d1 = glm::rotate(d0, 0.6f, glm::vec3(1, 0, 0));
	d1 = glm::scale(d1, glm::vec3(0.6f));
	i = glm::translate(i, glm::vec3(d0 * glm::vec4(0, 48, 0, 0)));
	d1 = i * d1;
	d1 = glm::scale(d1, glm::vec3(2, 6, 2));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(d1));
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);
	d1 = glm::scale(d1, glm::vec3(0.5, 0.167, 0.5));
	drawtree(clvao, clindices, d1, repeatnum, spline);

	glm::mat4 d2 = glm::rotate(d0, 0.6f, glm::vec3(-1, 0, 1.732));
	d2 = glm::scale(d2, glm::vec3(0.6f));
	d2 = i * d2;
	d2 = glm::scale(d2, glm::vec3(2, 6, 2));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(d2));
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);
	d2 = glm::scale(d2, glm::vec3(0.5, 0.167, 0.5));
	drawtree(clvao, clindices, d2, repeatnum, spline);

	glm::mat4 d3 = glm::rotate(d0, 0.6f, glm::vec3(-1, 0, -1.732));
	d3 = glm::scale(d3, glm::vec3(0.6f));
	d3 = i * d3;
	d3 = glm::scale(d3, glm::vec3(2, 6, 2));
	glUniformMatrix4fv(glGetUniformLocation(spline.ID, "model"), 1, GL_FALSE, glm::value_ptr(d3));
	glDrawElements(GL_TRIANGLES, clindices, GL_UNSIGNED_INT, NULL);
	d3 = glm::scale(d3, glm::vec3(0.5, 0.167, 0.5));
	drawtree(clvao, clindices, d3, repeatnum, spline);

}