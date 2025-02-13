#include <vector>

#include "sphere.h"

#ifndef CELESTIALBODIES_H
#define CELESTIALBODIES_H

using namespace std;

class CelestialBodies
{
	// systems move according to the sun and viceversa
	// subsystems move relative to the system's sun
	struct{
		float rotSp = 1.0f; // rotation speed
		float eccSp = 0.0f; // eccentricity speed
		float eccAg = 0.0f; // eccentricity angle
		glm::vec3 eccAx = glm::vec3(0.0f, 0.0f, 1.0f); // eccentricity axis
	} systemMov;
	struct subsystemMov {
		float orbSp = 1.0f; // orbit speed
		float orbRd = 10.0f; // orbit radius
		glm::vec3 orbAx = glm::vec3(0.0f, 1.0f, 0.0f); // orbit axis
		glm::vec3 orbCh = glm::vec3(0.0f, 0.0f, 0.0f); // orbit axis angular change velocity
		subsystemMov( // constructor
			float orbSp = 1.0f,
			float orbRd = 10.0f,
			glm::vec3 orbAx = glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3 orbCh = glm::vec3(0.0f, 0.0f, 0.0f)
		) {
			this->orbSp = orbSp;
			this->orbRd = orbRd;
			this->orbAx = orbAx;
			this->orbCh = orbCh;
		};
	};

public:
	CelestialBodies(
		double sunRd = 1.0,
		float rotSp = 1.0f,
		float eccSp = 0.0f,
		float eccAg = 0.0f,
		glm::vec3 eccAx = glm::vec3(0.0f, 1.0f, 0.0f)
	);
	~CelestialBodies();
	void addSubsystem(
		double sunRd = 1.0,
		float rotSp = 1.0f,
		float eccSp = 0.0f,
		float eccAg = 0.0f,
		glm::vec3 eccAx = glm::vec3(0.0f, 1.0f, 0.0f),
		float orbSp = 1.0f,
		float orbRd = 10.0f,
		glm::vec3 orbAx = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 orbCh = glm::vec3(0.0f, 0.0f, 0.0f)
	);
	vector<glm::vec3> getVertices();
	void getVertices_timeTransformed(
		glm::vec3** transformedVertices, 
		unsigned int* numVertices, 
		double t = 0.0,
		glm::mat4 transfMat = glm::mat4(1.0f)
	);
	vector<CelestialBodies> subsystems;

	bool getCamera(glm::vec3& cameraPosition, glm::vec3& cameraTarget, glm::vec3& upVector);


private:
	Sphere sun;
	//vector<CelestialBodies> subsystems;
	vector<subsystemMov> subsystemsMov;

};

#endif