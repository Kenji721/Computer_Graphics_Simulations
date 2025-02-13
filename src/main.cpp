/*
* CS535 - Lab 03
*
* Developed by
* Ramon Andres Galindo Gerardo (0248040@up.edu.mx)
* and
* Leonardo Kenji Minemura Suazo (0246757@up.edu.mx)
*
* Universidad Panamericana
* 2024
*/
/*
* Based on a framework created
* by Bedrich Benes, Ph. D.
* Purdue University
* bbenes@purdue.edu
* 2022
*/


/*
 Ruled surface
(C) Bedrich Benes 2022
Purdue University
bbenes@purdue.edu
*/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>            //Standard template library
#include <array>

#include "trackball.h"
#include "celestialBodies.h"

#pragma once
#pragma warning(disable : 4996)
#pragma comment(lib, "glfw3.lib")

using namespace std;

TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;

GLuint points = 0; //number of points to display the object
int steps = 12; //# of subdivisions
float pointSize = 5;
float lineWidth = 1;
GLdouble mouseX, mouseY;

float cameraDistance = 500.0f;
float cameraHeight = 0.0f;
float cameraAngle = 0.0f;

//Vertex array object and vertex buffer object indices 
GLuint VAO, VBO;

CelestialBodies solarSystem;
double ftime = 0.0f;
bool isPlaying = true;

inline void AddVertex(vector<GLfloat>* a, glm::vec3 v)
{
    a->push_back(v.x);
    a->push_back(v.y);
    a->push_back(v.z);
}

int CompileShaders() {
    //Vertex Shader
    const char* vsSrc = "#version 330 core\n"
        "layout (location = 0) in vec4 iPos;\n"
        "uniform mat4 modelview;\n"
        "void main()\n"
        "{\n"
        "   vec4 oPos=modelview*iPos;\n"
        "   gl_Position = oPos;\n"
        "}\0";

    //Fragment Shader
    const char* fsSrc = "#version 330 core\n"
        "out vec4 col;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "   col = color;\n"
        "}\n\0";

    //Create VS object
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    //Attach VS src to the Vertex Shader Object
    glShaderSource(vs, 1, &vsSrc, NULL);
    //Compile the vs
    glCompileShader(vs);

    //The same for FS
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, NULL);
    glCompileShader(fs);

    //Get shader program object
    GLuint shaderProg = glCreateProgram();
    //Attach both vs and fs
    glAttachShader(shaderProg, vs);
    glAttachShader(shaderProg, fs);
    //Link all
    glLinkProgram(shaderProg);

    //Clear the VS and FS objects
    glDeleteShader(vs);
    glDeleteShader(fs);
    return shaderProg;
}

void BuildScene(GLuint& VBO, GLuint& VAO, vector<glm::vec3>& positions) { //return VBO and VAO values

    // vector for shape vertices
    vector<GLfloat> v;
    glm::vec3* v_vec3_array = 0;
    unsigned int numVertices = 0;

    solarSystem.getVertices_timeTransformed(&v_vec3_array, &numVertices, ftime += 0.1);
    //for (unsigned int i = 0; i < numVertices; i++) {
    //    AddVertex(&v, v_vec3_array[i]);
    //    if (i % 10 == 0) { // Adjust the modulus value to get a representative vertex for each celestial body
    //        positions.push_back(v_vec3_array[i]);
    //    }
    //}
    
    // For Sun
    if (numVertices > 0) {
        positions.push_back(v_vec3_array[0]);
    }

    for (unsigned int i = 0; i < numVertices; i++) {
        AddVertex(&v, v_vec3_array[i]);
        if (i % 100 == 0) { // Adjust the modulus value to get representative vertices for celestial bodies
            positions.push_back(v_vec3_array[i]);
        }
    }
    /*
    // Capture positions of subsystems
    for (unsigned int i = 1; i < numVertices; i += 100) { 
        positions.push_back(v_vec3_array[i]);
    }

    for (unsigned int i = 0; i < numVertices; i++) {
        AddVertex(&v, v_vec3_array[i]);
    }
    */
    //make VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //bind it
    glBindVertexArray(VAO);

    //bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //send the data to the GPU
    points = (GLuint)v.size();
    glBufferData(GL_ARRAY_BUFFER, points * sizeof(GLfloat), &v[0], GL_STATIC_DRAW);

    //Configure the attributes
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //Make it valid
    glEnableVertexAttribArray(0);

    //no need for the data: it is in the GPU now
    delete[] v_vec3_array;
    v.clear();
}

//Quit when ESC is released
static void KbdCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//set the callbacks for the virtual trackball
//this is executed when the mouse is moving
void MouseCallback(GLFWwindow* window, double x, double y) {

    //do not forget to pass the events to ImGUI!
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)x, (float)y);
    if (io.WantCaptureMouse) return; //make sure you do not call this callback when over a menu

    //now process them
    mouseX = x;
    mouseY = y;
    //we need to perform an action only if a button is pressed
    if (mouseLeft)  trackball.Rotate((float)mouseX, (float)mouseY);
    if (mouseMid)   trackball.Translate((float)mouseX, (float)mouseY);
    if (mouseRight) trackball.Zoom((float)mouseX, (float)mouseY);
}

//set the variables when the button is pressed or released
void MouseButtonCallback(GLFWwindow* window, int button, int state, int mods) {
    //do not forget to pass the events to ImGUI!

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, state);
    if (io.WantCaptureMouse) return; //make sure you do not call this callback when over a menu

    //process them
    mouseLeft = !(button - GLFW_MOUSE_BUTTON_LEFT);
    mouseMid = !(button - GLFW_MOUSE_BUTTON_MIDDLE);
    mouseRight = !(button - GLFW_MOUSE_BUTTON_RIGHT);
    trackball.Set(window, !(state - GLFW_PRESS), (float)mouseX, (float)mouseY);
}

int main()
{
    glfwInit();

    //negotiate with the OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //make OpenGL window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Solar System", NULL, NULL);
    //is all OK?
    if (window == NULL)
    {
        std::cout << "Cannot open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //Paste the window to the current context
    glfwMakeContextCurrent(window);

    //Load GLAD to configure OpenGL
    gladLoadGL();
    //Set the viewport
    glViewport(0, 0, 800, 800);

    //SUN
    solarSystem = CelestialBodies(
        100.0f,
        1.0f,
        0.5f,
        10.f,
        glm::vec3(0.f, 0.f, 1.f)
    );
    //Planet1 
    solarSystem.addSubsystem(
        30.f,
        0.5f,
        0.1f,
        1.5f,
        glm::vec3(1.0f, 0.0f, 1.0f),
        -2.0f,
        250.0f,
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.1f, 0.1f, 0.1f)
    );
    //Planet 1 - Moon1
    solarSystem.subsystems[0].addSubsystem(
        8.f,    
        0.2f,   
        0.05f,  
        0.5f,   
        glm::vec3(0.5f, 0.0f, 0.5f), 
        20.0f,   
        60.0f,  
        glm::vec3(0.5f, 0.0f, 0.5f), 
        glm::vec3(0.05f, 0.05f, 0.05f)  
    );
    //Planet 2 
    solarSystem.addSubsystem(
        20.f,  
        0.3f,  
        0.2f,  
        1.0f,  
        glm::vec3(0.0f, 1.0f, 0.0f), 
        0.8f,  
        400.0f, 
        glm::vec3(0.0f, 1.0f, 0.0f), 
        glm::vec3(0.2f, 0.2f, 0.2f)  
    );
    // Planet 2 - Moon
    solarSystem.subsystems[1].addSubsystem(
        5.f,    
        0.1f,   
        0.03f,  
        0.4f,  
        glm::vec3(0.3f, 0.0f, 0.3f), 
        10.0f,   
        40.0f,  
        glm::vec3(0.3f, 0.0f, 0.3f), 
        glm::vec3(0.03f, 0.03f, 0.03f)  
    );
    //Planet 3
    solarSystem.addSubsystem(
        19.f,
        0.3f,
        0.2f,
        1.0f,  
        glm::vec3(0.0f, 1.0f, 0.0f), 
        0.3f,  
        600.0f, 
        glm::vec3(0.0f, 1.0f, 0.0f), 
        glm::vec3(0.2f, 0.2f, 0.2f)  
    );
    solarSystem.subsystems[2].addSubsystem(
        7.f,    
        0.1f,   
        0.03f,  
        0.4f,   
        glm::vec3(0.3f, 0.0f, 0.3f), 
        -20.0f,   
        50.0f,  
        glm::vec3(0.3f, 0.0f, 0.3f), 
        glm::vec3(0.03f, 0.03f, 0.03f)  
    );
    solarSystem.subsystems[2].addSubsystem(
        7.f,
        0.1f,
        0.03f,
        0.4f,
        glm::vec3(0.3f, 0.0f, 0.3f),
        -25.0f,
        90.0f,
        glm::vec3(0.3f, 0.0f, 0.3f),
        glm::vec3(0.03f, 0.03f, 0.03f)
    );

    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 500.f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    vector<glm::vec3> celestialPositions;

    BuildScene(VBO, VAO, celestialPositions);
    GLint shaderProg = CompileShaders();
    GLint modelviewParameter = glGetUniformLocation(shaderProg, "modelview");

    //Background color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //Use shader
    glUseProgram(shaderProg);
    glPointSize(pointSize);

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool drawScene = true;
    float color[4] = { 0.8f, 0.8f, 0.2f, 1.0f };
    //send the color to the fragment shader
    glUniform4f(glGetUniformLocation(shaderProg, "color"), color[0], color[1], color[2], color[3]);

    glfwSetKeyCallback(window, KbdCallback); //set keyboard callback to quit
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    const char* items[] = { "Sun", "Planet 1", "Moon of Planet 1", "Planet 2", "Moon of Planet 2", "Planet 3", "Moon of Planet 3", "Moone 2 of planet 3"};
    int item_current_idx = 0; // Index of current item
    float fov = 5000.0f;

    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        //Clean the window
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGUI window creation
        ImGui::Begin("Ruled Surface");
        //checkbox to render or not the scene
        ImGui::Checkbox("Draw Scene", &drawScene);

        celestialPositions.clear(); // Clear previous positions
        //color picker
        BuildScene(VBO, VAO, celestialPositions); //rebuild scene
        if (ImGui::SliderFloat("point Size", &pointSize, 1, 10, "%.1f", 0)) {
            glPointSize(pointSize); //set the new point size if it has been changed
        }
        if (ImGui::SliderFloat("line width", &lineWidth, 1, 10, "%.1f", 0)) {
            glLineWidth(lineWidth); //set the new point size if it has been changed
        }
        if (ImGui::ColorEdit4("Color", color)) { //set the new color only if it has changed
            glUniform4f(glGetUniformLocation(shaderProg, "color"), color[0], color[1], color[2], color[3]);
        }
        // Combo box to select the viewing position
        ImGui::Combo("Viewing Position", &item_current_idx, items, IM_ARRAYSIZE(items));
        // Slider to adjust the camera field of view
        ImGui::SliderFloat("Field of View", &fov, 1000.0f, 10000.0f);
        ImGui::SliderFloat("Camera Distance", &cameraDistance, 100.0f, 1000.0f);
        ImGui::SliderFloat("Camera Height", &cameraHeight, -1000.0f, 1000.0f);
        ImGui::SliderAngle("Camera Angle", &cameraAngle, -180.0f, 180.0f);

        glm::vec3 offset = glm::vec3(
            cameraDistance * cos(glm::radians(cameraAngle)),
            cameraHeight,
            cameraDistance * sin(glm::radians(cameraAngle))
        );

        if (!celestialPositions.empty()) {
            cameraTarget = celestialPositions[item_current_idx];
            cameraPosition = cameraTarget + offset;
        }

        // Update the camera position
        if (!celestialPositions.empty()) {
            switch (item_current_idx) {
            case 0: // Sun
                cameraPosition = glm::vec3(0.0f, 0.0f, 500.0f) + offset;
                break;
            case 1: // Planet 1
                if (celestialPositions.size() > 1) {
                    cameraPosition = celestialPositions[1] + offset;
                }
                break;
            case 2: // Moon of Planet 1
                if (celestialPositions.size() > 2) {
                    cameraPosition = celestialPositions[2] + offset;
                }
                break;
            case 3: // Planet 2
                if (celestialPositions.size() > 3) {
                    cameraPosition = celestialPositions[3] + offset; 
                }
                break;
            case 4: // Moon of Planet 2
                if (celestialPositions.size() > 4) {
                    cameraPosition = celestialPositions[4] + offset;
                }
                break;
            case 5: // Planet 3
                if (celestialPositions.size() > 5) {
                    cameraPosition = celestialPositions[5] + offset;
                }
                break;
            case 6: // Moon of Planet 3
                if (celestialPositions.size() > 6) {
                    cameraPosition = celestialPositions[6] + offset;
                }
                break;
            case 7: // Moon 2 of Planet 3
                if (celestialPositions.size() > 7) {
                    cameraPosition = celestialPositions[7] + offset;
                }
                break;
            }
        }
        // Ends the window
        ImGui::End();

        /*
        //set the projection matrix
        glm::mat4 modelView =
            glm::perspective(
                glm::radians(fov),  // Field of view
                1.0f,               // Aspect ratio
                0.01f, 1000.f       // Clipping planes
            ) *
            glm::lookAt(
                cameraPosition,     // Location of the camera
                glm::vec3(0, 0, 0), // Point the camera is looking at
                glm::vec3(0, 1, 0)  // Up vector of the camera (rotation)
            ) *
            trackball.Set3DViewCameraMatrix();
        // And send it to the vertex shader
        glUniformMatrix4fv(modelviewParameter, 1, GL_FALSE, glm::value_ptr(modelView));

        if (drawScene) {
            BuildScene(VBO, VAO);
            glDrawArrays(GL_POINTS, 0, points / 3);
            glDrawArrays(GL_TRIANGLES, 0, points / 3);
        }
        */

        glm::mat4 modelView =
            glm::perspective(glm::radians(fov), 1.0f, 0.01f, 10000.f) *
            glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0, 1, 0)) *
            trackball.Set3DViewCameraMatrix();

        glUniformMatrix4fv(modelviewParameter, 1, GL_FALSE, glm::value_ptr(modelView));

        if (drawScene) {
            BuildScene(VBO, VAO, celestialPositions);
            glDrawArrays(GL_POINTS, 0, points / 3);
            glDrawArrays(GL_TRIANGLES, 0, points / 3);
        }

        // Renders the ImGUI elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Make sure events are served
        glfwPollEvents();
    }

    //Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProg);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
