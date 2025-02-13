#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "celestialBodies.h"

CelestialBodies::CelestialBodies(
	double sunRd,
	float rotSp,
	float eccSp,
	float eccAg,
	glm::vec3 eccAx
) 
{
	sun = Sphere(sunRd);
	systemMov.rotSp = rotSp;
	systemMov.eccSp = eccSp;
	systemMov.eccAg = eccAg;
	systemMov.eccAx = glm::normalize(eccAx);
}

CelestialBodies::~CelestialBodies()
{

}

void CelestialBodies::addSubsystem(
	double sunRd,
	float rotSp,
	float eccSp,
	float eccAg,
	glm::vec3 eccAx,
	float orbSp,
	float orbRd,
	glm::vec3 orbAx,
	glm::vec3 orbCh
)
{
	subsystems.push_back(CelestialBodies(sunRd, rotSp, eccSp, eccAg, eccAx));
	subsystemsMov.push_back(subsystemMov(orbSp, orbRd, glm::normalize(orbAx), orbCh));
}

vector<glm::vec3> CelestialBodies::getVertices()
{
	vector<glm::vec3> v = sun.getVertices();
	return v;
}

void CelestialBodies::getVertices_timeTransformed(
	glm::vec3** transformedVertices,
	unsigned int* numVertices,
	double t,
	glm::mat4 transfMat
)
{
	vector<glm::vec3> v;
	glm::vec3* temp_v = 0;
	unsigned int temp_numV = 0;
	//double* angle = new double;
	double angle;
	//glm::mat4* systemTransfMat = new glm::mat4;
	//glm::mat4* subsystemTransfMat = new glm::mat4;
	glm::mat4 systemTransfMat = transfMat;
	glm::mat4 subsystemTransfMat;

	// rotation
	angle = systemMov.rotSp * t;
	systemTransfMat = glm::rotate(transfMat, (float)angle, glm::vec3(0.f, 1.f, 0.f));
	// eccentricity
	systemTransfMat = glm::rotate(systemTransfMat, systemMov.eccAg, systemMov.eccAx);

	// sun
	sun.getVertices_transformed(&temp_v, &temp_numV, systemTransfMat);
	for (unsigned int i = 0; i < temp_numV; i++)
		v.push_back(temp_v[i]);
	delete[] temp_v;

	// subsystems
	for (unsigned int i = 0; i < subsystems.size(); i++)
	{

		/*glm::vec4 temp_vec4 = glm::vec4(
			subsystemsMov[i].orbAx.x,
			subsystemsMov[i].orbAx.y,
			subsystemsMov[i].orbAx.z,
			1.0f
		);
		*angle = subsystemsMov[i].orbCh.x * t;
		temp_vec4 = glm::rotate(glm::mat4(1.0f), (float)*angle, glm::vec3(1.f, 0.f, 0.f)) * temp_vec4;
		*angle = subsystemsMov[i].orbCh.y * t;
		temp_vec4 = glm::rotate(glm::mat4(1.0f), (float)*angle, glm::vec3(0.f, 1.f, 0.f)) * temp_vec4;
		*angle = subsystemsMov[i].orbCh.z * t;
		temp_vec4 = glm::rotate(glm::mat4(1.0f), (float)*angle, glm::vec3(0.f, 0.f, 1.f)) * temp_vec4;
		subsystemsMov[i].orbAx = glm::vec3(temp_vec4.x, temp_vec4.y, temp_vec4.z);*/

		/*
		*angle = (M_PI / 2.0) - acos(subsystemsMov[i].orbCh.x);
		*subsystemTransfMat = glm::rotate(*systemTransfMat, (float)*angle, glm::vec3(0.f, 0.f, 1.f));
		*angle = (M_PI / 2.0) - acos(subsystemsMov[i].orbCh.z);
		*subsystemTransfMat = glm::rotate(*subsystemTransfMat, (float)*angle, glm::vec3(0.f, 1.f, 0.f));
		*angle = (M_PI / 2.0) - acos(subsystemsMov[i].orbCh.y);
		*subsystemTransfMat = glm::rotate(*subsystemTransfMat, (float)*angle, glm::vec3(1.f, 0.f, 0.f));

		*angle = subsystemsMov[i].orbSp * t;
		*subsystemTransfMat = glm::rotate(*subsystemTransfMat, (float)*angle, glm::vec3(0.f, 1.f, 0.f));

		*subsystemTransfMat = glm::translate(*subsystemTransfMat, glm::vec3(subsystemsMov[i].orbRd, 0.f, 0.f));

		subsystems[i].getVertices_timeTransformed(&temp_v, &temp_numV, t, *subsystemTransfMat);
		for (unsigned int i = 0; i < temp_numV; i++)
			v.push_back(temp_v[i]);
		delete[] temp_v;
		*/
		angle = (M_PI / 2.0) - acos(subsystemsMov[i].orbCh.x);
		subsystemTransfMat = glm::rotate(systemTransfMat, (float)angle, glm::vec3(0.f, 0.f, 1.f));
		angle = (M_PI / 2.0) - acos(subsystemsMov[i].orbCh.z);
		subsystemTransfMat = glm::rotate(subsystemTransfMat, (float)angle, glm::vec3(0.f, 1.f, 0.f));
		angle = (M_PI / 2.0) - acos(subsystemsMov[i].orbCh.y);
		subsystemTransfMat = glm::rotate(subsystemTransfMat, (float)angle, glm::vec3(1.f, 0.f, 0.f));

		angle = subsystemsMov[i].orbSp * t;
		subsystemTransfMat = glm::rotate(subsystemTransfMat, (float)angle, glm::vec3(0.f, 1.f, 0.f));

		subsystemTransfMat = glm::translate(subsystemTransfMat, glm::vec3(subsystemsMov[i].orbRd, 0.f, 0.f));

		subsystems[i].getVertices_timeTransformed(&temp_v, &temp_numV, t, subsystemTransfMat);
		for (unsigned int j = 0; j < temp_numV; j++)
			v.push_back(temp_v[j]);
		delete[] temp_v;
	}

	*numVertices = v.size();
	*transformedVertices = new glm::vec3[*numVertices];
	for (unsigned int i = 0; i < *numVertices; i++)
		(*transformedVertices)[i] = v[i];

	//delete angle;
	//delete systemTransfMat;
	//delete subsystemTransfMat;
	v.clear();
}

bool CelestialBodies::getCamera(glm::vec3& cameraPosition, glm::vec3& cameraTarget, glm::vec3& upVector) {
	glm::vec3* transformedVertices = nullptr;
	unsigned int numVertices = 0;

	this->getVertices_timeTransformed(&transformedVertices, &numVertices, 0, glm::mat4(1.0f));
	if (numVertices == 0) {
		delete[] transformedVertices;
		return false;
	}

	glm::vec3 centroid(0.0f);
	for (unsigned int i = 0; i < numVertices; ++i) {
		centroid += transformedVertices[i];
	}
	centroid /= numVertices;

	cameraPosition = centroid + glm::vec3(500.0f, 500.0f, 500.0f);
	cameraTarget = centroid;
	upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	delete[] transformedVertices;
	return true;
}