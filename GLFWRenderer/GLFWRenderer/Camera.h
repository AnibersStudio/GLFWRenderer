#pragma once
#include "GLCommon.h"

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& eye, float hor = 0.0f, float vet = 0.0f);

	void Rotate(float hor, float vert);//Modifier
	void Move(float toward, float right);

	const glm::vec3 & GetEye() const;//Getter
	const glm::vec3 & GetTarget() const;
	const glm::vec3 & GetUp() const;
	glm::mat4 GetLookat();
	const glm::vec3 & GetSight() const;
private:
	void Update();
	const glm::vec3 EyeOrigin = glm::vec3(0.0, 0.0, 0.0);//The origin is the same as OpenGL
	const glm::vec3 UpOrigin = glm::vec3(0.0, 1.0, 0.0);
	const glm::vec3 SightOrigin = glm::vec3(0.0, 0.0, -1.0);
	float horizonalangle = 0.0;
	float verticalangle = 0.0;
	glm::vec3 Eye = EyeOrigin;
	glm::vec3 Sight = SightOrigin;
	glm::vec3 Up = UpOrigin;
	glm::vec3 Target = Eye + Sight;
	bool shouldupdate = true;
};