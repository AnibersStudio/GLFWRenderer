#pragma once
#include "GLCommon.h"

class Camera
{
public:
	/// <summary> Default camera. Same as OpenGL default </summary>
	Camera() = default;
	/// <summary> Camera constructor </summary>
	Camera(const glm::vec3& eye, float hor = 0.0f, float vet = 0.0f);

	/// <summary> Rotate you FPS camera 'hor' degrees horizonaly and 'vert' degrees verticly </summary>
	void Rotate(float hor, float vert);
	/// <summary> Move you FPS camera toward 'toward' unit and right 'right' unit </summary>
	void Move(float toward, float right);

	/// <summary> Get the eye position of camera </summary>
	const glm::vec3 & GetEye();
	/// <summary> Get the target position of camera </summary>
	const glm::vec3 & GetTarget();
	/// <summary> Get the up vector of camera </summary>
	const glm::vec3 & GetUp();
	/// <summary> Get the sight vector of camera pointing from eye to target </summary>
	const glm::vec3 & GetSight();
	/// <summary> Get the view matrix </summary>
	glm::mat4 GetLookat();
private:
	void Update();
	const glm::vec3 EyeOrigin = glm::vec3(0.0, 0.0, 0.0);
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