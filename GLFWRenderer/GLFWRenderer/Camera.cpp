#include "Camera.h"
Camera::Camera()
{
}

Camera::Camera(const glm::vec3 & eye, float hor, float vet) : Eye(eye), horizonalangle(hor), verticalangle(vet)
{
	Update();
}

void Camera::Rotate(float hor, float vert)
{
	horizonalangle += hor;
	verticalangle += vert;
	verticalangle = glm::clamp(verticalangle, -90.0f, 90.0f);
	horizonalangle = horizonalangle > 360.0f ? horizonalangle - 360.0f : horizonalangle;
	horizonalangle = horizonalangle < 0.0f ? horizonalangle + 360.0f : horizonalangle;
	shouldupdate = true;
}

void Camera::Move(float toward, float right)
{
	glm::vec3 rightaxis = glm::cross(Sight, Up);
	Eye += toward * Sight;
	Eye += right * rightaxis;
	shouldupdate = true;
}

const glm::vec3 & Camera::GetEye() const
{
	return Eye;
}

const glm::vec3 & Camera::GetTarget() const
{

	return Target;
}

const glm::vec3 & Camera::GetUp() const
{
	return Up;
}

glm::mat4 Camera::GetLookat()
{
	if (shouldupdate) Update();
	return glm::lookAt(Eye, Target, Up);
}

const glm::vec3 & Camera::GetSight() const
{
	return Sight;
}

void Camera::Update()
{
	Sight = glm::vec3((glm::rotate(glm::mat4(1.0), horizonalangle, UpOrigin) * glm::vec4(SightOrigin, 1.0f)));
	Sight = glm::normalize(Sight);
	glm::vec3 horizonalaxis = glm::cross(Sight, UpOrigin);
	glm::mat4 vertrotate = glm::rotate(glm::mat4(1.0), verticalangle, horizonalaxis);
	Sight = glm::vec3(((vertrotate)* glm::vec4(Sight, 1.0)));
	Sight = glm::normalize(Sight);
	Target = Eye + Sight;//target refreshed

	Up = glm::vec3((vertrotate * glm::vec4(UpOrigin, 1.0)));
	Up = glm::normalize(Up);//up refreshed

	shouldupdate = false;
}
