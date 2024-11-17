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
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"

#include "MathHelper.h"


#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif


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
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		//glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	//GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	//GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	//GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat lightgrayLight[] = { 0.7f, 0.7f, 0.7f, 1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };
	GLfloat darkLight[] = { .1f, .1f, .1f, 1.0 };
	GLfloat blackLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	//glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	//glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);
	glShadeModel(GL_SMOOTH);

	//set light0, the main light
	float light0_position[] = { 200.0f, 200.0f, 100.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, darkLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, grayLight);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, grayLight);

	//set light1, the first control point light
	float light1_position[] = { m_pTrack->points[0].pos.x, m_pTrack->points[0].pos.y, m_pTrack->points[0].pos.z, 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, blackLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0001f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0001f);

	//set light2, the train headlight
	float light2_position[] = { trainPos.x, trainPos.y, trainPos.z, 1.0 };
	float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular[] = { 1.0,1.0,1.0,1.0 };
	//properties of the light
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT2, GL_AMBIENT, blackLight);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, yellowLight);
	glLightfv(GL_LIGHT2, GL_SPECULAR, yellowLight);
	/*Spot properties*/
	//spot direction
	float light2_direction[] = { trainFront.x, trainFront.y, trainFront.z };
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_direction);
	//angle of the cone light emitted by the spot : value between 0 to 180
	float spotCutOff = 45;
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotCutOff);
	//exponent propertie defines the concentration of the light
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 50.0f);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0001f);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	//glDisable(GL_LIGHTING);
	drawFloor(200, 200);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
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
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this, aspect);
#endif
		//glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(100, aspect, 0.01, 200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(trainPos.x, trainPos.y, trainPos.z, trainPos.x + trainFront.x, trainPos.y + trainFront.y, trainPos.z + trainFront.z, trainUp.x, trainUp.y, trainUp.z);
	}
}

void GxM(float* points, float* matrix) {
	float tempPoints[4] = { points[0],points[1],points[2],points[3] };
	for (int i = 0; i < 4; i++) {
		points[i] = 0;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			points[i] += matrix[j * 4 + i] * tempPoints[j];
		}
	}
}

float MxT(float* matrix, float t) {
	float result = 0;
	float points[4] = { t * t * t,t * t,t,1 };
	for (int i = 0; i < 4; i++) {
		result += points[i] * matrix[i];
	}
	return result;
}

void mulRotateMatrix(float* matrix, Pnt3f& points) {
	float tempPoints[3] = { points.x,points.y,points.z };
	float* pointsP[3] = { &points.x,&points.y,&points.z };
	for (int i = 0; i < 3; i++) {
		*pointsP[i] = 0;
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			*pointsP[i] += matrix[j * 3 + i] * tempPoints[j];
		}
	}
}

/*
material:	0 = shape
			1 = metal
			2 = plastic
*/


void TrainView::
drawCube(Pnt3f pos, Pnt3f front, Pnt3f right, Pnt3f up, float w, float h, float l, float r, float g, float b, int material, bool doingShadows, bool debug = 0) {
	Pnt3f vertexes[8];
	char minus[8][3]{
	{-1,-1, 1 },
	{ 1,-1, 1 },
	{ 1,-1,-1 },
	{-1,-1,-1 },
	{-1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 1,-1 },
	{-1, 1,-1 }
	};

	for (int i = 0; i < 8; i++) {
		vertexes[i].x = minus[i][0] * w / 2;
		vertexes[i].y = minus[i][1] * h / 2;
		vertexes[i].z = minus[i][2] * l / 2;
	}

	front.normalize();
	right.normalize();
	up.normalize();
	float rotateMatrix[9] = {
		right.x, right.y, right.z,
		up.x, up.y, up.z,
		front.x, front.y, front.z,
	};
	for (int i = 0; i < 8; i++) {
		mulRotateMatrix(rotateMatrix, vertexes[i]);
	}
	for (int i = 0; i < 8; i++) {
		vertexes[i].x += pos.x;
		vertexes[i].y += pos.y;
		vertexes[i].z += pos.z;
	}
	Pnt3f* surface[6][4]{
	{&vertexes[0],&vertexes[3],&vertexes[2],&vertexes[1]},
	{&vertexes[0],&vertexes[1],&vertexes[5],&vertexes[4]},
	{&vertexes[1],&vertexes[2],&vertexes[6],&vertexes[5]},
	{&vertexes[2],&vertexes[3],&vertexes[7],&vertexes[6]},
	{&vertexes[3],&vertexes[0],&vertexes[4],&vertexes[7]},
	{&vertexes[4],&vertexes[5],&vertexes[6],&vertexes[7]}
	};
	glShadeModel(GL_SMOOTH);
	if (!doingShadows) {

		glColor3ub(r, g, b);
		GLfloat materialAmbient[] = { r, g, b, 1.0 };
		GLfloat materialDiffuse[] = { r, g, b, 1.0 };
		GLfloat materialShininess = MathHelper::lerp(0, 128.f, 0.7);
		GLfloat materialSpecular[] = { 1.0 * (materialShininess / 128.0f), 1.0 * (materialShininess / 128.0f), 1.0 * (materialShininess / 128.0f), 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
	}
	for (int i = 0; i < 6; i++) {
		if (debug && !doingShadows) {
			if (i == 1)
				glColor3ub(180, 0, 0);
			else if (i == 2)
				glColor3ub(0, 180, 0);
			else if (i == 5)
				glColor3ub(0, 0, 180);
			else
				glColor3ub(r, g, b);
		}
		glBegin(GL_QUADS);
		Pnt3f normal = (*surface[i][3] + -1 * *surface[i][0]) * (*surface[i][1] + -1 * *surface[i][0]);
		normal.normalize();
		Pnt3f diagonal1(*surface[i][0] + -1 * *surface[i][2]);
		Pnt3f diagonal2(*surface[i][1] + -1 * *surface[i][3]);
		diagonal1.normalize();
		diagonal2.normalize();
		diagonal1 = diagonal1 * 0.2;
		diagonal2 = diagonal2 * 0.2;
		if (debug && 0) {
			//printf("%f %f %f\n", surface[i][0]->x, surface[i][0]->y, surface[i][0]->z);
			printf("%f %f %f\n", normal.x, normal.y, normal.z);
			printf("%f %f %f\n", normal.x + diagonal1.x, normal.y + diagonal1.y, normal.z + diagonal1.z);
			printf("%f %f %f\n", normal.x + diagonal2.x, normal.y + diagonal2.y, normal.z + diagonal2.z);
		}
		if (material == MATERIAL_PLASTIC)
			glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f);
		if (material == MATERIAL_METAL)
			glNormal3f(normal.x + diagonal1.x, normal.y + diagonal1.y, normal.z + diagonal1.z);
		glVertex3f(surface[i][0]->x, surface[i][0]->y, surface[i][0]->z);
		glTexCoord2f(1.0f, 0.0f);
		if (material == MATERIAL_METAL)
			glNormal3f(normal.x + diagonal2.x, normal.y + diagonal2.y, normal.z + diagonal2.z);
		glVertex3f(surface[i][1]->x, surface[i][1]->y, surface[i][1]->z);
		glTexCoord2f(1.0f, 1.0f);
		if (material == MATERIAL_METAL)
			glNormal3f(normal.x - diagonal1.x, normal.y - diagonal1.y, normal.z - diagonal1.z);
		glVertex3f(surface[i][2]->x, surface[i][2]->y, surface[i][2]->z);
		glTexCoord2f(0.0f, 1.0f);
		if (material == MATERIAL_METAL)
			glNormal3f(normal.x - diagonal2.x, normal.y - diagonal2.y, normal.z - diagonal2.z);
		glVertex3f(surface[i][3]->x, surface[i][3]->y, surface[i][3]->z);
		glEnd();
	}
}

void TrainView::
drawSleeper(Pnt3f pos, Pnt3f front, Pnt3f right, bool doingShadows) {
	front.normalize();
	right.normalize();
	Pnt3f up = right * front;
	drawCube(pos, front, right, up, 10, 0.5, 2, 255, 255, 255,1, doingShadows);
}

void TrainView::
drawTrain(Pnt3f pos, Pnt3f front, Pnt3f right, bool doingShadows) {
	front.normalize();
	right.normalize();
	Pnt3f up = right * front;
	pos = pos + 4.5 * up;
	drawCube(pos, front, right, up, 6, 8, 10, 128, 128, 180,1, doingShadows, 1);
}

void TrainView::
drawTrack(Pnt3f start, Pnt3f end, Pnt3f right, bool doingShadows) {
	Pnt3f center = (start + end) * 0.5;
	Pnt3f difference = end + start * -1;
	drawCube(center, difference, right, right * difference, 0.3, 0.3, difference.len()+0.05, 109, 72, 55,2, doingShadows);
}

void TrainView::
drawTree(Pnt3f pos, float rotateTheta, bool doingShadows, float treeTrunkWidth, float treeHeight, float leafHeight, float leafWidth, float leafWidthDecreaseDelta) {
	//check to make sure tree is look good
	if (leafWidth - (treeHeight / 2.0f / leafHeight) * leafWidthDecreaseDelta < treeTrunkWidth) {
		leafWidth = (treeHeight / 2.0f / leafHeight) * leafWidthDecreaseDelta + treeTrunkWidth;
	}

	int treeTrunkColor[] = { 92, 64, 3 };
	int leafColor[] = { 12, 171, 23 };

	const Pnt3f ORIGIN = Pnt3f(0, 0, 0);
	const Pnt3f UP = Pnt3f(0, 1, 0);
	const Pnt3f FRONT = Pnt3f(0, 0, -1);
	const Pnt3f RIGHT = Pnt3f(1, 0, 0);

	int meterial = MATERIAL_PLASTIC;

	//transform
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rotateTheta, 0, 1, 0);

	//draw trunk
	drawCube(ORIGIN + UP * (treeHeight / 2.0f), FRONT, RIGHT, UP, treeTrunkWidth, treeHeight, treeTrunkWidth, treeTrunkColor[0], treeTrunkColor[1], treeTrunkColor[2], meterial, doingShadows);

	//draw leaf
	for (Pnt3f baseHeight = ORIGIN + UP * (treeHeight / 2.0f) + UP * (leafHeight / 2.0f); baseHeight.y <= treeHeight + leafHeight; baseHeight = baseHeight + UP * leafHeight) {
		drawCube(baseHeight, FRONT, RIGHT, UP, leafWidth, leafHeight, leafWidth, leafColor[0], leafColor[1], leafColor[2], meterial, doingShadows);
		leafWidth -= leafWidthDecreaseDelta;
	}

	glPopMatrix();
	
	
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

	//draw the tree
	drawTree(Pnt3f(0, 0, 0), 0, doingShadows);

	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
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
		float* M;
		float cp_pos_x[4] = { cp_pos_p0.x,cp_pos_p1.x,cp_pos_p2.x,cp_pos_p3.x };
		float cp_pos_y[4] = { cp_pos_p0.y,cp_pos_p1.y,cp_pos_p2.y,cp_pos_p3.y };
		float cp_pos_z[4] = { cp_pos_p0.z,cp_pos_p1.z,cp_pos_p2.z,cp_pos_p3.z };
		float cp_orient_x[4] = { cp_orient_p0.x,cp_orient_p1.x,cp_orient_p2.x,cp_orient_p3.x };
		float cp_orient_y[4] = { cp_orient_p0.y,cp_orient_p1.y,cp_orient_p2.y,cp_orient_p3.y };
		float cp_orient_z[4] = { cp_orient_p0.z,cp_orient_p1.z,cp_orient_p2.z,cp_orient_p3.z };
		if (tw->splineBrowser->value() == TrainWindow::LINEAR) { //linear
			M = new float[16]{
				0,0,0,0,
				0,0,-1,1,
				0,0,1,0,
				0,0,0,0
			};
			for (int i = 0; i < 16; i++) {
				M[i] /= 1.0f;
			}
		}
		else if (tw->splineBrowser->value() == TrainWindow::CARDINAL) { //cardinal
			M = new float[16]{
				-1,2,-1,0,
				3,-5,0,2,
				-3,4,1,0,
				1,-1,0,0
			};
			for (int i = 0; i < 16; i++) {
				M[i] /= 2.0f;
			}
		}
		else { // B-spline
			M = new float[16]{
				-1,3,-3,1,
				3,-6,0,4,
				-3,3,3,1,
				1,0,0,0
			};
			for (int i = 0; i < 16; i++) {
				M[i] /= 6.0f;
			}
		}
		GxM(cp_pos_x, M);
		GxM(cp_pos_y, M);
		GxM(cp_pos_z, M);
		GxM(cp_orient_x, M);
		GxM(cp_orient_y, M);
		GxM(cp_orient_z, M);
		delete[] M;
		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		Pnt3f qt(MxT(cp_pos_x, t), MxT(cp_pos_y, t), MxT(cp_pos_z, t));
		

		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			Pnt3f qt0 = qt;
			t += percent;
			qt = Pnt3f(MxT(cp_pos_x, t), MxT(cp_pos_y, t), MxT(cp_pos_z, t));
			Pnt3f qt1 = qt;
			glLineWidth(1);
			if (!doingShadows)
				glColor3ub(32, 32, 64);
			Pnt3f orient_t(MxT(cp_orient_x, t), MxT(cp_orient_y, t), MxT(cp_orient_z, t));
			orient_t.normalize();
			Pnt3f cross_t = ((qt1 + qt0 * -1) * orient_t);
			cross_t.normalize();
			cross_t = cross_t * (track_width / 2);
			drawTrack(qt0 + cross_t, qt1 + cross_t, cross_t, doingShadows);
			drawTrack(qt0 + cross_t * -1, qt1 + cross_t * -1, cross_t, doingShadows);
			Pnt3f distance = qt1 + (-1 * last_sleeper);
			bool needToDrawTrain = false;
			if (distance.len() > 5) {
				drawSleeper(qt1, qt1 + qt0 * -1, cross_t, doingShadows);
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
				glLightfv(GL_LIGHT2, GL_POSITION, position2);
				glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction);
				if (!tw->trainCam->value())
					drawTrain(qt1, trainFront, cross_t, doingShadows);
				trainDrawed = true;
			}
		}
	}
	totalArcLength = presentArcLength;

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################




#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
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