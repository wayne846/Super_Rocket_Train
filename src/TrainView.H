/************************************************************************
     File:        TrainView.H

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

#pragma once

// Preclarify for preventing the compiler error
class TrainWindow;
class CTrack;


//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#include <vector>
#include <string>
#pragma warning(pop)

// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"
#include "Utilities/Pnt3f.H"

#include "RenderUnit/Shader.h"
#include "RenderUnit/RenderStructure.h"
#include "RenderUnit/InstanceDrawer.h"

class TrainView : public Fl_Gl_Window
{
	public:
		
		// note that we keep the "standard widget" constructor arguments
		TrainView(int x, int y, int w, int h, const char* l = 0);

		// overrides of important window things
		virtual int handle(int);
		virtual void draw();

		// all of the actual drawing happens in this routine
		// it has to be encapsulated, since we draw differently if
		// we're drawing shadows (no colors, for example)
		void drawStuff(bool doingShadows=false);

		// setup the projection - assuming that the projection stack has been
		// cleared for you
		void setProjection();

		// Reset the Arc ball control
		void resetArcball();

		// pick a point (for when the mouse goes down)
		void doPick();
	private:
		void initRander();
		void initLight(); //init all light to dark(black)(0 ,0, 0)
		void setShaders();

		void drawSimpleObject(const Object& object, const glm::mat4 model, const Material material);
		void drawTree(glm::vec3 pos, float rotateTheta = 0.0f, float treeTrunkWidth = 7.0f, float treeHeight = 40.0f, float leafHeight = 10.0f, float leafWidth = 20.0f, float leafWidthDecreaseDelta = 5.0f);
		void drawWater(glm::vec3 pos, glm::vec3 scale, float rotateTheta = 0.0f);

		void aim();
	public:
		const float DIVIDE_LINE = 128; //how many segment are track divide
		const int MATERIAL_SHAPE = 0;
		const int MATERIAL_METAL = 1;
		const int MATERIAL_PLASTIC = 2;

		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube;  // simple - just remember which cube is selected
		float			t_time = 0;
		
		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene

	private:
		std::vector<float> arcLength;
		float totalArcLength = 0;
		Pnt3f trainPos;
		Pnt3f trainFront;
		Pnt3f trainUp;
		Pnt3f trainRight;

		// aiming
		float camRotateX = 0,camRotateY = 0;
		float lastX=0, lastY=0;	// the mouse position

		//all light in the scene
		DirLight dirLight;
		PointLight pointLights[4];
		SpotLight spotLights[4];

		//something about shader
		bool hasInitRander = false;
		Shader* simpleObjectShader;
		Shader* simpleInstanceObjectShader;
		Shader* waterShader;
		Object cube;
		Object water; //1x1 plane

		//texture
		unsigned int waterHeightMap[200];
		unsigned int waterNormalMap[200];
		unsigned int skybox;
};