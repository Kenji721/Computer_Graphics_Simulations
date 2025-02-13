#include <vector>

#include <glad/glad.h>
#include "glm/glm.hpp"

#ifndef M_PI
#define M_PI 3.141592
#endif

#ifndef SPHERE_H
#define SPHERE

using namespace std;

class Sphere
{

public:
	Sphere(double r = 1.0f, unsigned int numStacks = 10, unsigned int numSlices = 10);
	~Sphere();
	vector<glm::vec3> getVertices();
	void getVertices_transformed(glm::vec3** transformedVertices, unsigned int* numVertices, glm::mat4 transfMat);

private:
	double r = 1.0f;
	vector<glm::vec3> vertices;
	void calculate(unsigned int stacks, unsigned int slices);

};

#endif