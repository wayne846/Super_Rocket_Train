/************************************************************************
	 File:        TrainView.cpp

	 Author:
				  Michael Gleicher, gleicher@cs.wisc.edu

	 Modifier
				  Yu-Chi Lai, yu-chi@cs.wisc.edu

	 Comment:
						The TrainView is the window that actually shows the
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within
						a TrainWindow
						that is the outer window with all the widgets.
						The TrainView needs
						to be aware of the window - since it might need to
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know
						about it (beware circular references)

	 Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <algorithm>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GL/glu.h"

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"

#include "MathHelper.h"

#define SIMPLE_OBJECT_VERT_PATH "/assets/shaders/simpleObject.vert"
#define SIMPLE_OBJECT_FRAG_PATH "/assets/shaders/simpleObject.frag"
#define INSTANCE_OBJECT_VERT_PATH "/assets/shaders/instanceObject.vert"
#define WATER_VERT_PATH "/assets/shaders/water.vert"
#define WATER_FRAG_PATH "/assets/shaders/water.frag"

#define WATER_HEIGHT_PATH "/assets/images/waterHeight/"
#define WATER_NORMAL_PATH "/assets/images/waterNormal/"

const std::vector<std::string> SKYBOX_PATH = {
	PROJECT_DIR "/assets/images/skybox/right.jpg",
	PROJECT_DIR "/assets/images/skybox/left.jpg",
	PROJECT_DIR "/assets/images/skybox/top.jpg",
	PROJECT_DIR "/assets/images/skybox/bottom.jpg",
	PROJECT_DIR "/assets/images/skybox/front.jpg",
	PROJECT_DIR "/assets/images/skybox/back.jpg"
};

#define WATER_RESOLUTION 120

//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l)
	: Fl_Gl_Window(x, y, w, h, l)
	//========================================================================
{
	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);

	resetArcball();
	
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event))
			return 1;

	// remember what button was used
	static int last_push;

	switch (event) {
		// Mouse button being pushed event
	case FL_PUSH:
		last_push = Fl::event_button();
		// if the left button be pushed is left mouse button
		if (last_push == FL_LEFT_MOUSE) {
			doPick();
			damage(1);
			return 1;
		};
		break;

		// Mouse button release event
	case FL_RELEASE: // button release
		damage(1);
		last_push = 0;
		return 1;

		// Mouse button drag event
	case FL_DRAG:

		// Compute the new control point position
		if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
			ControlPoint* cp = &m_pTrack->points[selectedCube];

			double r1x, r1y, r1z, r2x, r2y, r2z;
			getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

			double rx, ry, rz;
			mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
				static_cast<double>(cp->pos.x),
				static_cast<double>(cp->pos.y),
				static_cast<double>(cp->pos.z),
				rx, ry, rz,
				(Fl::event_state() & FL_CTRL) != 0);

			cp->pos.x = (float)rx;
			cp->pos.y = (float)ry;
			cp->pos.z = (float)rz;
			damage(1);
		}
		break;

		// in order to get keyboard events, we need to accept focus
	case FL_FOCUS:
		return 1;

		// every time the mouse enters this window, aggressively take focus
	case FL_ENTER:
		focus(this);
		break;

	case FL_KEYBOARD:
		int k = Fl::event_key();
		int ks = Fl::event_state();
		if (k == 'p') {
			// Print out the selected control point information
			if (selectedCube >= 0)
				printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
					selectedCube,
					m_pTrack->points[selectedCube].pos.x,
					m_pTrack->points[selectedCube].pos.y,
					m_pTrack->points[selectedCube].pos.z,
					m_pTrack->points[selectedCube].orient.x,
					m_pTrack->points[selectedCube].orient.y,
					m_pTrack->points[selectedCube].orient.z);
			else
				printf("Nothing Selected\n");

			return 1;
		};
		break;
	}

	return Fl_Gl_Window::handle(event);
}

//init shader, texture, VAO. need called under if(gladLoadGL())
void TrainView::initRander() {
	//init shader
	simpleObjectShader = new Shader(PROJECT_DIR SIMPLE_OBJECT_VERT_PATH, PROJECT_DIR SIMPLE_OBJECT_FRAG_PATH);
	simpleInstanceObjectShader = new Shader(PROJECT_DIR INSTANCE_OBJECT_VERT_PATH, PROJECT_DIR SIMPLE_OBJECT_FRAG_PATH);
	waterShader = new Shader(PROJECT_DIR WATER_VERT_PATH, PROJECT_DIR WATER_FRAG_PATH);

	//init texture
	for (int i = 0; i < 200; i++) {
		std::string zero = "00";
		if (i >= 10 && i < 100) zero = "0";
		if (i >= 100) zero = "";
		waterHeightMap[i] = RenderDatabase::loadTexture(PROJECT_DIR WATER_HEIGHT_PATH + (zero + std::to_string(i) + ".png"));
	}
	for (int i = 0; i < 200; i++) {
		std::string zero = "00";
		if (i >= 10 && i < 100) zero = "0";
		if (i >= 100) zero = "";
		waterNormalMap[i] = RenderDatabase::loadTexture(PROJECT_DIR WATER_NORMAL_PATH + (zero + std::to_string(i) + "_normal.png"));
	}
	skybox = RenderDatabase::loadCubemap(SKYBOX_PATH);

	//init VAO
	//------------------
	//cube
	GLfloat cubeVertices[] = {
		//down
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		//front -z
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		//right +x
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		//back
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		//left
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		//top +y
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f
	};
	GLfloat cubeNormal[] = {
		//down
		0, -1.0f, 0,
		0, -1.0f, 0,
		0, -1.0f, 0,
		0, -1.0f, 0,
		//front -z
		0, 0, -1.0f,
		0, 0, -1.0f,
		0, 0, -1.0f,
		0, 0, -1.0f,
		//right +x
		1.0f, 0, 0,
		1.0f, 0, 0,
		1.0f, 0, 0,
		1.0f, 0, 0,
		//back
		0, 0, 1.0f,
		0, 0, 1.0f,
		0, 0, 1.0f,
		0, 0, 1.0f,
		//left
		-1.0f, 0, 0,
		-1.0f, 0, 0,
		-1.0f, 0, 0,
		-1.0f, 0, 0,
		//top +y
		0, 1.0f, 0,
		0, 1.0f, 0,
		0, 1.0f, 0,
		0, 1.0f, 0
	};
	GLuint cubeElement[] = {
		//down
		0, 1, 2,
		0, 2, 3,
		//front -z
		4, 5, 6,
		4, 6, 7,
		//right +x
		8, 9, 10,
		8, 10, 11,
		//back
		12, 13, 14,
		12, 14, 15,
		//left
		16, 17, 18,
		16, 18, 19,
		//top +y
		20, 21, 22,
		20, 22, 23,
	};
	glGenVertexArrays(1, &cube.VAO);
	glGenBuffers(2, cube.VBO);
	glGenBuffers(1, &cube.EBO);
	glBindVertexArray(cube.VAO);
	cube.element_amount = sizeof(cubeElement) / sizeof(GLuint);
	// Position attribute
	glBindBuffer(GL_ARRAY_BUFFER, cube.VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, cube.VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormal), cubeNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	//Element attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeElement), cubeElement, GL_STATIC_DRAW);
	// Unbind VAO
	glBindVertexArray(0);

	//water
	GLfloat  waterVertices[WATER_RESOLUTION * WATER_RESOLUTION * 3];
	GLfloat  waterTexcoords[WATER_RESOLUTION * WATER_RESOLUTION * 2];
	GLuint waterElement[(WATER_RESOLUTION - 1) * (WATER_RESOLUTION - 1) * 6];
	for (int i = 0; i < WATER_RESOLUTION; i++) {
		for (int j = 0; j < WATER_RESOLUTION; j++) {
			int t = (i * WATER_RESOLUTION + j) * 3;
			waterVertices[t] = j / (float)(WATER_RESOLUTION - 1) - 0.5f;
			waterVertices[t + 1] = 0;
			waterVertices[t + 2] = i / (float)(WATER_RESOLUTION - 1) - 0.5f;
		}
	}
	for (int i = 0; i < WATER_RESOLUTION; i++) {
		for (int j = 0; j < WATER_RESOLUTION; j++) {
			int t = (i * WATER_RESOLUTION + j) * 2;
			waterTexcoords[t] = j / (float)(WATER_RESOLUTION - 1);
			waterTexcoords[t + 1] = i / (float)(WATER_RESOLUTION - 1);
		}
	}
	for (int i = 0; i < WATER_RESOLUTION; i++) {
		for (int j = 0; j < WATER_RESOLUTION; j++) {
			if (i == WATER_RESOLUTION - 1 || j == WATER_RESOLUTION - 1) continue;
			int t = (i * (WATER_RESOLUTION - 1) + j) * 6;
			int p = i * WATER_RESOLUTION + j;
			waterElement[t] = p;
			waterElement[t + 1] = p + WATER_RESOLUTION + 1;
			waterElement[t + 2] = p + 1;
			waterElement[t + 3] = p;
			waterElement[t + 4] = p + WATER_RESOLUTION;
			waterElement[t + 5] = p + WATER_RESOLUTION + 1;
		}
	}
	glGenVertexArrays(1, &water.VAO);
	glGenBuffers(2, water.VBO);
	glGenBuffers(1, &water.EBO);
	glBindVertexArray(water.VAO);
	water.element_amount = sizeof(waterElement) / sizeof(GLuint);
	// Position attribute
	glBindBuffer(GL_ARRAY_BUFFER, water.VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Texcoords attribute
	glBindBuffer(GL_ARRAY_BUFFER, water.VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterTexcoords), waterTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	//Element attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, water.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(waterElement), waterElement, GL_STATIC_DRAW);
	// Unbind VAO
	glBindVertexArray(0);
}

//set all light to dark
void TrainView::initLight() {
	const glm::vec3 ZERO = glm::vec3(0, 0, 0);
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const float ONE_FLOAT = 1.0f;
	const float ZERO_FLOAT = 0.0f;
	//set directional light
	dirLight.ambient = ZERO;
	dirLight.diffuse = ZERO;
	dirLight.specular = ZERO;
	dirLight.direction = UP;
	//set point light
	for (int i = 0; i < 4; i++) {
		pointLights[i].ambient = ZERO;
		pointLights[i].diffuse = ZERO;
		pointLights[i].specular = ZERO;
		pointLights[i].position = ZERO;
		pointLights[i].constant = ONE_FLOAT;
		pointLights[i].linear = ZERO_FLOAT;
		pointLights[i].quadratic = ZERO_FLOAT;
	}
	//set spot light
	for (int i = 0; i < 4; i++) {
		spotLights[i].ambient = ZERO;
		spotLights[i].diffuse = ZERO;
		spotLights[i].specular = ZERO;
		spotLights[i].position = ZERO;
		spotLights[i].direction = UP;
		spotLights[i].cutOff = ZERO_FLOAT;
		spotLights[i].outerCutOff = ZERO_FLOAT;
		spotLights[i].constant = ONE_FLOAT;
		spotLights[i].linear = ZERO_FLOAT;
		spotLights[i].quadratic = ZERO_FLOAT;
	}
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
		if (!hasInitRander) {
			initRander();
			hasInitRander = true;
		}
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, .3f, 0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	initLight();
	//point light 0, main light
	pointLights[0].position = glm::vec3(200.0f, 200.0f, 100.0f);
	pointLights[0].ambient = RenderDatabase::GRAY_COLOR;
	pointLights[0].diffuse = RenderDatabase::LIGHT_GRAY_COLOR;
	pointLights[0].specular = RenderDatabase::WHITE_COLOR;
	//point light 1, first controlpoint light
	glm::vec3 firstContropointPos = glm::vec3(m_pTrack->points[0].pos.x, m_pTrack->points[0].pos.y, m_pTrack->points[0].pos.z);
	pointLights[1].position = firstContropointPos;
	pointLights[1].diffuse = RenderDatabase::WHITE_COLOR;
	pointLights[1].specular = RenderDatabase::WHITE_COLOR;
	pointLights[1].linear = 0.014f;
	pointLights[1].quadratic = 0.0007f;
	//spot light 0, train headlight
	glm::vec3 trainHeadlightPos = trainPos.glmvec3() + trainFront.glmvec3() * 5.1f + trainUp.glmvec3() * 4.0f;
	spotLights[0].position = trainHeadlightPos;
	spotLights[0].direction = trainFront.glmvec3();
	spotLights[0].diffuse = RenderDatabase::YELLOW_COLOR;
	spotLights[0].specular = RenderDatabase::YELLOW_COLOR;
	spotLights[0].cutOff = cos(MathHelper::degreeToRadians(30));
	spotLights[0].outerCutOff = cos(MathHelper::degreeToRadians(35));
	spotLights[0].linear = 0.007; 
	spotLights[0].quadratic = 0.0002;

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	//glDisable(GL_LIGHTING);
	//drawFloor(200, 200);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	/*
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}*/
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else if (tw->trainCam->value()) {
		//glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(100, aspect, 0.01, 200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(trainPos.x, trainPos.y, trainPos.z, trainPos.x + trainFront.x, trainPos.y + trainFront.y, trainPos.z + trainFront.z, trainUp.x, trainUp.y, trainUp.z);
	}
}

//set shader uniform, like view, projection, lights...
void TrainView::setShaders() {
	//get view matrix and projection matrix
	glm::mat4 view;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);
	glm::mat4 projection;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	//set simpleObjectShader and simpleInstanceObjectShader
	Shader* shaders[] = { simpleObjectShader, simpleInstanceObjectShader, waterShader };
	int size = sizeof(shaders) / sizeof(Shader*);
	for (int i = 0; i < size; i++) {
		shaders[i]->use();

		//set the uniform
		shaders[i]->setMat4("view", view);
		shaders[i]->setMat4("projection", projection);

		glm::vec3 eyepos = glm::vec3(view[0][2] * -arcball.getEyePos().z, view[1][2] * -arcball.getEyePos().z, view[2][2] * -arcball.getEyePos().z);
		shaders[i]->setVec3("eyePosition", eyepos);

		// light properties
		shaders[i]->setVec3("dirLight.ambient", dirLight.ambient);
		shaders[i]->setVec3("dirLight.diffuse", dirLight.diffuse);
		shaders[i]->setVec3("dirLight.specular", dirLight.specular);
		shaders[i]->setVec3("dirLight.direction", dirLight.direction);
		for (int j = 0; j < 4; j++) {
			shaders[i]->setVec3("pointLights[" + std::to_string(j) + "].ambient", pointLights[j].ambient);
			shaders[i]->setVec3("pointLights[" + std::to_string(j) + "].diffuse", pointLights[j].diffuse);
			shaders[i]->setVec3("pointLights[" + std::to_string(j) + "].specular", pointLights[j].specular);
			shaders[i]->setVec3("pointLights[" + std::to_string(j) + "].position", pointLights[j].position);
			shaders[i]->setFloat("pointLights[" + std::to_string(j) + "].constant", pointLights[j].constant);
			shaders[i]->setFloat("pointLights[" + std::to_string(j) + "].linear", pointLights[j].linear);
			shaders[i]->setFloat("pointLights[" + std::to_string(j) + "].quadratic", pointLights[j].quadratic);
		}
		for (int j = 0; j < 4; j++) {
			shaders[i]->setVec3("spotLights[" + std::to_string(j) + "].ambient", spotLights[j].ambient);
			shaders[i]->setVec3("spotLights[" + std::to_string(j) + "].diffuse", spotLights[j].diffuse);
			shaders[i]->setVec3("spotLights[" + std::to_string(j) + "].specular", spotLights[j].specular);
			shaders[i]->setVec3("spotLights[" + std::to_string(j) + "].position", spotLights[j].position);
			shaders[i]->setVec3("spotLights[" + std::to_string(j) + "].direction", spotLights[j].direction);
			shaders[i]->setFloat("spotLights[" + std::to_string(j) + "].cutOff", spotLights[j].cutOff);
			shaders[i]->setFloat("spotLights[" + std::to_string(j) + "].outerCutOff", spotLights[j].outerCutOff);
			shaders[i]->setFloat("spotLights[" + std::to_string(j) + "].constant", spotLights[j].constant);
			shaders[i]->setFloat("spotLights[" + std::to_string(j) + "].linear", spotLights[j].linear);
			shaders[i]->setFloat("spotLights[" + std::to_string(j) + "].quadratic", spotLights[j].quadratic);
		}
	}
	
	//set back to opengl fixed pipeline
	glUseProgram(0);
}

//draw object by simple object shader
void TrainView::drawSimpleObject(const Object& object, const glm::mat4 model, const Material material) {
	simpleObjectShader->use();

	simpleObjectShader->setMat4("model", model);
	simpleObjectShader->setMat4("normalMatrix", glm::transpose(glm::inverse(model)));	

	// material properties
	simpleObjectShader->setVec3("material.ambient", material.ambient);
	simpleObjectShader->setVec3("material.diffuse", material.diffuse);
	simpleObjectShader->setVec3("material.specular", material.specular);
	simpleObjectShader->setFloat("material.shininess", material.shininess);

	glBindVertexArray(object.VAO);
	glDrawElements(GL_TRIANGLES, object.element_amount, GL_UNSIGNED_INT, 0);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);
}

//roatateTheta is degree and anticlockwise by +y
void TrainView::
drawTree(glm::vec3 pos, float rotateTheta, float treeTrunkWidth, float treeHeight, float leafHeight, float leafWidth, float leafWidthDecreaseDelta) {
	//check to make sure tree is look good
	if (leafWidth - (treeHeight / 2.0f / leafHeight) * leafWidthDecreaseDelta < treeTrunkWidth) {
		leafWidth = (treeHeight / 2.0f / leafHeight) * leafWidthDecreaseDelta + treeTrunkWidth;
	}

	Material treeTrunkMaterial = {
		glm::vec3(0.36f, 0.25f, 0.011f),
		glm::vec3(0.36f, 0.25f, 0.011f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		10.0f
	};
	Material leafMaterial = {
		glm::vec3(0.047f, 0.67f, 0.011f),
		glm::vec3(0.047f, 0.67f, 0.011f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		10.0f
	};

	const glm::vec3 ORIGIN = pos;
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const glm::vec3 FRONT = glm::vec3(sin(MathHelper::degreeToRadians(rotateTheta)), 0, -cos(MathHelper::degreeToRadians(rotateTheta)));

	//draw trunk
	glm::mat4 treeTrunkModel = MathHelper::getTransformMatrix(ORIGIN + UP * (treeHeight / 2.0f), FRONT, UP, glm::vec3(treeTrunkWidth, treeHeight, treeTrunkWidth));
	drawSimpleObject(cube, treeTrunkModel, treeTrunkMaterial);

	//draw leaf
	for (glm::vec3 baseHeight = ORIGIN + UP * (treeHeight / 2.0f) + UP * (leafHeight / 2.0f); baseHeight.y <= treeHeight + leafHeight; baseHeight = baseHeight + UP * leafHeight) {
		glm::mat4 leafModel = MathHelper::getTransformMatrix(baseHeight, FRONT, UP, glm::vec3(leafWidth, leafHeight, leafWidth));
		drawSimpleObject(cube, leafModel, leafMaterial);
		leafWidth -= leafWidthDecreaseDelta;
	}
}

void TrainView::drawWater(glm::vec3 pos, glm::vec3 scale, float rotateTheta) {
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const glm::vec3 FRONT = glm::vec3(sin(MathHelper::degreeToRadians(rotateTheta)), 0, -cos(MathHelper::degreeToRadians(rotateTheta)));
	glm::mat4 model = MathHelper::getTransformMatrix(pos, FRONT, UP, scale);

	Material waterMaterial = {
		glm::vec3(0.25f, 0.52f, 0.96f),
		glm::vec3(0.25f, 0.52f, 0.96f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		128.0f
	};

	waterShader->use();

	waterShader->setMat4("model", model);
	waterShader->setMat4("normalMatrix", glm::transpose(glm::inverse(model)));

	// material properties
	waterShader->setVec3("material.ambient", waterMaterial.ambient);
	waterShader->setVec3("material.diffuse", waterMaterial.diffuse);
	waterShader->setVec3("material.specular", waterMaterial.specular);
	waterShader->setFloat("material.shininess", waterMaterial.shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterHeightMap[tw->clock_time % 200]);
	waterShader->setInt("heightMap", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterNormalMap[tw->clock_time % 200]);
	waterShader->setInt("normalMap", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	waterShader->setInt("skybox", 2);

	glBindVertexArray(water.VAO);
	glDrawElements(GL_TRIANGLES, water.element_amount, GL_UNSIGNED_INT, 0);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);
}

//************************************************************************
// cp_pos_p0.x,cp_pos_p1.x,cp_pos_p2.x,cp_pos_p3.x };
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	//set up shaders uniform
	setShaders();

	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}

	//draw the floor
	glm::mat4 floorModel = MathHelper::getTransformMatrix(glm::vec3(0, -0.5, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(200, 1, 200));
	drawSimpleObject(cube, floorModel, RenderDatabase::GREEN_PLASTIC_MATERIAL);

	//draw the tree
	//drawTree(glm::vec3(0, 0, 0));

	//draw the water
	drawWater(glm::vec3(0, 1, 0), glm::vec3(50, 1, 50));

	// draw the track, sleeper, train
	Material trainMaterial = {
		glm::vec3(0.19225f, 0.19225f, 0.89225f),
		glm::vec3(0.50754f, 0.50754f, 0.80754f),
		glm::vec3(0.508273f, 0.508273f, 0.808273f),
		128.0f
	};
	InstanceDrawer trackInstance(RenderDatabase::SLIVER_MATERIAL);
	InstanceDrawer sleeperInstance(RenderDatabase::SLIVER_MATERIAL);
	InstanceDrawer trainInstance(trainMaterial);
	const float track_width = 5;
	Pnt3f last_sleeper(999, 999, 999);
	int num_point = m_pTrack->points.size();

	bool trainDrawed = false;
	float presentArcLength = 0;
	
	for (int i = 0; i < num_point; ++i) {

		// pos
		Pnt3f cp_pos_p0 = m_pTrack->points[(i + num_point - 1) % num_point].pos;
		Pnt3f cp_pos_p1 = m_pTrack->points[i].pos;
		Pnt3f cp_pos_p2 = m_pTrack->points[(i + 1) % num_point].pos;
		Pnt3f cp_pos_p3 = m_pTrack->points[(i + 2) % num_point].pos;
		// orient
		Pnt3f cp_orient_p0 = m_pTrack->points[(i + num_point - 1) % num_point].orient;
		Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
		Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % num_point].orient;
		Pnt3f cp_orient_p3 = m_pTrack->points[(i + 2) % num_point].orient;
		
		float cp_pos_x[4] = { cp_pos_p0.x,cp_pos_p1.x,cp_pos_p2.x,cp_pos_p3.x };
		float cp_pos_y[4] = { cp_pos_p0.y,cp_pos_p1.y,cp_pos_p2.y,cp_pos_p3.y };
		float cp_pos_z[4] = { cp_pos_p0.z,cp_pos_p1.z,cp_pos_p2.z,cp_pos_p3.z };
		float cp_orient_x[4] = { cp_orient_p0.x,cp_orient_p1.x,cp_orient_p2.x,cp_orient_p3.x };
		float cp_orient_y[4] = { cp_orient_p0.y,cp_orient_p1.y,cp_orient_p2.y,cp_orient_p3.y };
		float cp_orient_z[4] = { cp_orient_p0.z,cp_orient_p1.z,cp_orient_p2.z,cp_orient_p3.z };

		float M[16];
		float linearMatrix[16] = {
			0,0,0,0,
			0,0,-1,1,
			0,0,1,0,
			0,0,0,0
		};
		float cardinalMatrix[16] = {
			-1,2,-1,0,
			3,-5,0,2,
			-3,4,1,0,
			1,-1,0,0
		};
		float bSplineMatrix[16] = {
			-1,3,-3,1,
			3,-6,0,4,
			-3,3,3,1,
			1,0,0,0
		};
		if (tw->splineBrowser->value() == TrainWindow::LINEAR) { //linear
			std::copy(std::begin(linearMatrix), std::end(linearMatrix), std::begin(M));
			for (int i = 0; i < 16; i++) {
				M[i] /= 1.0f;
			}
		}
		else if (tw->splineBrowser->value() == TrainWindow::CARDINAL) { //cardinal
			std::copy(std::begin(cardinalMatrix), std::end(cardinalMatrix), std::begin(M));
			for (int i = 0; i < 16; i++) {
				M[i] /= 2.0f;
			}
		}
		else { // B-spline
			std::copy(std::begin(bSplineMatrix), std::end(bSplineMatrix), std::begin(M));
			for (int i = 0; i < 16; i++) {
				M[i] /= 6.0f;
			}
		}
		MathHelper::GxM(cp_pos_x, M);
		MathHelper::GxM(cp_pos_y, M);
		MathHelper::GxM(cp_pos_z, M);
		MathHelper::GxM(cp_orient_x, M);
		MathHelper::GxM(cp_orient_y, M);
		MathHelper::GxM(cp_orient_z, M);

		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		Pnt3f qt(MathHelper::MxT(cp_pos_x, t), MathHelper::MxT(cp_pos_y, t), MathHelper::MxT(cp_pos_z, t));
		
		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			Pnt3f qt0 = qt;
			t += percent;
			qt = Pnt3f(MathHelper::MxT(cp_pos_x, t), MathHelper::MxT(cp_pos_y, t), MathHelper::MxT(cp_pos_z, t));
			Pnt3f qt1 = qt;
			glLineWidth(1);
			if (!doingShadows)
				glColor3ub(32, 32, 64);
			Pnt3f orient_t(MathHelper::MxT(cp_orient_x, t), MathHelper::MxT(cp_orient_y, t), MathHelper::MxT(cp_orient_z, t));
			orient_t.normalize();
			Pnt3f cross_t = ((qt1 + qt0 * -1) * orient_t);
			cross_t.normalize();
			cross_t = cross_t * (track_width / 2);
			//draw track
			Pnt3f trackCenter1 = (qt0 + cross_t + qt1 + cross_t) * 0.5f;
			Pnt3f trackCenter2 = (qt0 + cross_t * -1 + qt1 + cross_t * -1) * 0.5;
			Pnt3f trackDifference = qt1 - qt0;
			glm::mat4 trackModel1 = MathHelper::getTransformMatrix(trackCenter1.glmvec3(), trackDifference.glmvec3(), (cross_t * trackDifference).glmvec3(), glm::vec3(0.3, 0.3, trackDifference.len() + 0.05));
			glm::mat4 trackModel2 = MathHelper::getTransformMatrix(trackCenter2.glmvec3(), trackDifference.glmvec3(), (cross_t * trackDifference).glmvec3(), glm::vec3(0.3, 0.3, trackDifference.len() + 0.05));
			trackInstance.addModelMatrix(trackModel1);
			trackInstance.addModelMatrix(trackModel2);

			Pnt3f distance = qt1 + (-1 * last_sleeper);
			bool needToDrawTrain = false;
			if (distance.len() > 5) {
				//draw sleeper
				glm::vec3 up = glm::cross(cross_t.glmvec3(), (qt1 + qt0 * -1).glmvec3());
				glm::mat4 sleeperModel = MathHelper::getTransformMatrix(qt1.glmvec3(), (qt1 + qt0 * -1).glmvec3(), up, glm::vec3(10, 0.5, 2));
				sleeperInstance.addModelMatrix(sleeperModel);

				last_sleeper = qt1;
			}
			if (tw->arcLength->value() == false) {
				if (t_time * m_pTrack->points.size() >= i + t && t_time * m_pTrack->points.size() <= i + t + percent)
					needToDrawTrain = true;
			}
			else  {
				presentArcLength += (qt1 + (-1 * qt0)).len();
				if (!trainDrawed && presentArcLength / totalArcLength >= t_time)
					needToDrawTrain = true;
			}
			if (needToDrawTrain) {
				trainFront = qt1 + qt0 * -1;
				trainFront.normalize();
				trainUp = cross_t * trainFront;
				trainUp.normalize();
				trainPos = Pnt3f(qt1 + trainUp * 3);
				Pnt3f trainLightPosition = qt1 + trainFront * 5.1 + trainUp * 1;
				float position2[] = { qt1.x + trainFront.x, qt1.y + trainFront.y, qt1.z + trainFront.z, 1.0f };
				float direction[] = { trainFront.x,trainFront.y,trainFront.z };
				if (!tw->trainCam->value()) {
					//draw train
					glm::vec3 right = cross_t.glmvec3();
					glm::vec3 up = glm::cross(glm::normalize(right), trainFront.glmvec3());
					glm::vec3 glmpos = qt1.glmvec3();
					glmpos = glmpos + 4.5f * up;
					glm::mat4 model = MathHelper::getTransformMatrix(glmpos, trainFront.glmvec3(), up, glm::vec3(6, 8, 10));
					trainInstance.addModelMatrix(model);
				}	
				trainDrawed = true;
			}
		}
	}
	totalArcLength = presentArcLength;
	trackInstance.drawByInstance(simpleInstanceObjectShader, cube);
	sleeperInstance.drawByInstance(simpleInstanceObjectShader, cube);
	trainInstance.drawByInstance(simpleInstanceObjectShader, cube);

	//draw axis
	glLineWidth(5);
	glBegin(GL_LINES);
	if (!doingShadows) {
		glColor3f(1, 0, 0);
	}
	glVertex3f(0, 0, 0);
	glVertex3f(20, 0, 0);
	if (!doingShadows) {
		glColor3f(0, 1, 0);
	}
	glVertex3f(0, 0, 0);
	glVertex3f(0, 20, 0);
	if (!doingShadows) {
		glColor3f(0, 0, 1);
	}
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 20);
	glEnd();
	glLineWidth(1);
	
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();

	// where is the mouse?
	int mx = Fl::event_x();
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my),
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		glLoadName((GLuint)(i + 1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3] - 1;
	}
	else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n", selectedCube);
}