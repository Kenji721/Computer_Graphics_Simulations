#include "sphere.h"

Sphere::Sphere(double r, unsigned int numStacks, unsigned int numSlices)
{
	Sphere::r = r;
	calculate(numStacks, numSlices);
}

Sphere::~Sphere() 
{
	vertices.clear();
}

void Sphere::calculate(unsigned int stacks, unsigned int slices)
{

	glm::vec3* currentSquare_vertices = new glm::vec3[4];

	GLfloat deltaTheta = 2 * M_PI / (GLfloat)slices;
	GLfloat deltaPhi = M_PI / (GLfloat)stacks;

	for (GLint i = 0; i < stacks; i++)
	{
		GLfloat phi = i * deltaPhi;
		for (GLint j = 0; j < slices; j++)
		{
			GLfloat theta = j * deltaTheta;

			currentSquare_vertices[0] = glm::vec3(
				r * cos(theta) * sin(phi),
				r * sin(theta) * sin(phi),
				r * cos(phi)
			);
			currentSquare_vertices[1] = glm::vec3(
				r * cos(theta + deltaTheta) * sin(phi),
				r * sin(theta + deltaTheta) * sin(phi),
				r * cos(phi)
			);
			currentSquare_vertices[2] = glm::vec3(
				r * cos(theta) * sin(phi + deltaPhi),
				r * sin(theta) * sin(phi + deltaPhi),
				r * cos(phi + deltaPhi)
			);
			currentSquare_vertices[3] = glm::vec3(
				r * cos(theta + deltaTheta) * sin(phi + deltaPhi),
				r * sin(theta + deltaTheta) * sin(phi + deltaPhi),
				r * cos(phi + deltaPhi)
			);

			//the first triangle
			vertices.push_back(currentSquare_vertices[0]);
			vertices.push_back(currentSquare_vertices[1]);
			vertices.push_back(currentSquare_vertices[2]);
			//the second triangle
			vertices.push_back(currentSquare_vertices[1]);
			vertices.push_back(currentSquare_vertices[2]);
			vertices.push_back(currentSquare_vertices[3]);

		}
	}

	delete[] currentSquare_vertices;
}

vector<glm::vec3> Sphere::getVertices()
{
	return vertices;
}

void Sphere::getVertices_transformed(
	glm::vec3** transformedVertices,
	unsigned int* numVertices,
	glm::mat4 transfMat
)
{
	glm::vec4 currentVertex;

	*numVertices = vertices.size();
	*transformedVertices = new glm::vec3[*numVertices];
	for (unsigned int i = 0; i < *numVertices; i++)
	{
		currentVertex = transfMat * glm::vec4((vertices[i]).x, (vertices[i]).y, (vertices[i]).z, 1.0f);
		(*transformedVertices)[i] = glm::vec3(currentVertex.x, currentVertex.y, currentVertex.z);
	}
}