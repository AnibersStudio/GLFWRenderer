#include "Vertex.h"

void Vertex::Transform(const glm::mat4& transformmatrix)
{
	glm::vec4 pos4 = transformmatrix * glm::vec4(position, 1.0f);
	position = glm::vec3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);
	glm::vec4 normal4 = glm::transpose(glm::inverse(transformmatrix)) * glm::vec4(normal, 1.0);
	normal = glm::normalize(glm::vec3(normal4));
	glm::vec4 tangent4 = transformmatrix * glm::vec4(tangent, 1.0f);
	tangent = glm::normalize(glm::vec3(tangent4));
}
