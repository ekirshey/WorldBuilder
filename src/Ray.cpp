#include "Ray.h"

Ray::Ray() {

}

Ray::Ray(float norm_x, float norm_y, glm::vec3 origin, glm::mat4 proj, glm::mat4 view) 
{
	reset(norm_x, norm_y, origin, proj, view);
}

void Ray::reset(float norm_x, float norm_y, glm::vec3 origin, glm::mat4 proj, glm::mat4 view) {
	_origin = origin;
	glm::vec4 ray_clip(norm_x, norm_y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(proj) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	_direction = glm::inverse(view) * ray_eye;
	_direction = glm::normalize(_direction);
}


float Ray::intersectWithPlane(glm::vec3 plane_normal, glm::vec3 plane_offset) {
	float o_dot_n = glm::dot(_origin, plane_normal);
	float d_dot_n = glm::dot(_direction, plane_normal);
	return -1 * (o_dot_n) / (d_dot_n);
}

// http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Ray::intersectWithTriangle(glm::vec3 V0, glm::vec3 V1, glm::vec3 V2) {
	glm::vec3 E1 = V1 - V0;
	glm::vec3 E2 = V2 - V0;
	glm::vec3 T = _origin - V0;
	glm::vec3 P = glm::cross(_direction, E2);
	glm::vec3 Q = glm::cross(T, E1);

	glm::vec3 tuv = (1 / (glm::dot(P, E1))) * glm::vec3(glm::dot(Q, E2), glm::dot(P, T), glm::dot(Q, _direction));

	if (tuv.y >= 0 && tuv.z >= 0 && (tuv.y + tuv.z) <= 1) {
		return true;
	}

	return false;
}

bool Ray::intersectWithPoint(glm::vec3 point, float ray_magnitude) {
	glm::vec3 ray_pos = _origin + _direction*ray_magnitude;
	if ((ray_pos.x <= point.x + 0.001) && (ray_pos.x >= point.x - 0.001) &&
		(ray_pos.y <= point.y + 0.001) && (ray_pos.y >= point.y - 0.001) &&
		(ray_pos.z <= point.z + 0.001) && (ray_pos.z >= point.z - 0.001)
		) 
	{
		return true;
	}
	else {
		return false;
	}
}

float distance(const Ray& a, const Ray& b) {
	return glm::distance(a.direction(), b.direction());
}
