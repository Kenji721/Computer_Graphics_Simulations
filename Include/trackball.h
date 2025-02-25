/*
 Virtual trackball for glfw
(C) Bedrich Benes 2022
Purdue University
bbenes@purdue.edu
*/


#ifndef TRACKBALL_H
#define TRACKBALL_H


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926
#endif


class TrackBallC
{
public:
	TrackBallC()	//the constructor resets the modelview to identity
	{
		glm::mat4 modelview(1.0);
	}

	~TrackBallC()
	{}

	void SetWH(GLFWwindow* window)	//we need to know the window size
	{
		glfwGetWindowSize(window, &w, &h);
	}

	//remember the coordinates when the mouse is pressed
	void Set(GLFWwindow* window, bool mouse, float x, float y)
	{
		SetWH(window); //get the window size if it has changed
		mouseDown=mouse;
		if (mouseDown)//mouse pressed, remember the coordinates
		{
		 //coordinates are relative to the previous location
		  lastPoint=trackBall(x,y);
		  oldX=x;//need to store oldX and oldY for zoom and translate
		  oldY=y;
		} 
	}

	void Rotate(float x, float y)
	{
		if (mouseDown)
		{
		   glm::vec3 curPoint=trackBall(x,y);
		   glm::vec3 direction=curPoint-lastPoint;
		   GLfloat angle = direction.length() * 1.2f;//times its sensitivity
		   glm::vec3 axis = glm::cross(lastPoint, curPoint);
		   axis=glm::normalize(axis);
		   glm::mat4 rot = glm::rotate(glm::mat4(1.0), angle, axis); //make the rotation matrix
		   float trX = modelview[3][0];//store the translation 
		   float trY = modelview[3][1];
		   float trZ = modelview[3][2];
		   modelview[3][0] = 0; 
		   modelview[3][1] = 0;
		   modelview[3][2] = 0;
		   //watch out! We need to FIRST rotate and then multiply to roate around 
		   modelview = rot*modelview; //rotate only around the origin
		   modelview[3][0] = trX;
		   modelview[3][1] = trY;
		   modelview[3][2] = trZ;
		   lastPoint=curPoint;
		}
	}

	glm::mat4 Set3DViewCameraMatrix()  //return the modelview
	{
		return(modelview);
	}

	void Zoom(float x, float y)
	{
		if (mouseDown)
		{
			GLfloat translateZ=(y-oldY)*0.01f;
			glm::mat4 tmp= glm::translate(glm::mat4(1.0), glm::vec3(0, 0, translateZ));
			//perform the transformation and then apply the previous modelview
			modelview = tmp*modelview;
			oldX=x;
			oldY=y;
		}
	}

	glm::vec3 trackBall(float x, float y)
	{
		double xx = (2.f * x - w) / w;
		double yy = (h - 2.f * y) / h;
		double d = sqrt(xx*xx+yy*yy);
		d = (d < 1.f) ? d : 1.f;  // If d is > 1, then clamp it at one.
		return glm::normalize(glm::vec3(xx, yy, sqrt(1.001 - d * d)));
	}

	void Translate(float x, float y)
	{
		if (mouseDown)
		{
			GLfloat transX=(x-oldX)*0.01f;
			GLfloat transY=(1-(y-oldY))*0.01f;
			glm::mat4 tmp= glm::translate(glm::mat4(1.0), glm::vec3(transX, transY, 0));
			//perform the transformation and then apply the previous modelview
			modelview = tmp*modelview;
			oldX=x;
			oldY=y;
		}
	}

private:
		glm::vec3 lastPoint = glm::vec3(0.0f);
		GLfloat startX = (GLfloat)0.0f;
		GLfloat startY = (GLfloat)0.0f;
		GLfloat endX = (GLfloat)0.0f;
		GLfloat endY = (GLfloat)0.0f;
		GLfloat oldX = (GLfloat)0.0f;
		GLfloat oldY = (GLfloat)0.0f;
		//the main window
		GLfloat moveX = (GLfloat)0.0f; 
		GLfloat moveY = (GLfloat)0.0f;
		bool mouseDown = false;
		glm::mat4 modelview = glm::mat4(1.0f);
		//windowW and windowH
		GLint w = (GLint)0;
		GLint h = (GLint)0;
};

#endif