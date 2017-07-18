#include "Ray.h"

//temp
#include <iostream>

#include <glm/ext.hpp>

Ray::Ray() {

}

Ray::Ray(float norm_x, float norm_y, glm::vec3 origin, glm::mat4 proj, glm::mat4 view) 
{
	reset(norm_x, norm_y, origin, proj, view);
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction)
	: _origin(origin)
	, _direction(direction)
{
}

void Ray::reset(float norm_x, float norm_y, glm::vec3 origin, glm::mat4 proj, glm::mat4 view) {
	_origin = origin;
	glm::vec4 ray_clip(norm_x, norm_y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(proj) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	_direction = glm::inverse(view) * ray_eye;
	_direction = glm::normalize(_direction);
}

// http://antongerdelan.net/opengl/raycasting.html
// intersection with INFINITE plane
bool Ray::intersectWithPlane(const glm::vec3& plane_normal, const glm::vec3& plane_offset, float& ray_magnitude) const {
	bool ret = true;
	float d_dot_n = glm::dot(_direction, plane_normal);

	// Ray is parallel to plane
	// i.e. direction dot normal = 0
	if (std::fabs(d_dot_n) <= 1e-6) {
		ray_magnitude = 0;
		return false;
	}

	float o_dot_n = glm::dot( -_origin, plane_normal);

	ray_magnitude =  (o_dot_n) / (d_dot_n);

	if (ray_magnitude < 0) {
		ret = false;
	}

	return ret;
}

// http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Ray::intersectWithTriangle(const glm::vec3& V0, const glm::vec3& V1, const glm::vec3& V2) const {
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

glm::vec3 Ray::atPoint(float t) const
{
	return _origin + _direction*t;
}

bool Ray::intersectWithPoint(const glm::vec3& point, float ray_magnitude) const {
	glm::vec3 ray_pos = atPoint(ray_magnitude);
	// Replace .001 with "epsilon"
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

namespace RayUtils {
	// https://stackoverflow.com/questions/29195120/determine-whether-a-point-is-inside-triangle-in-3d-space
	// http://blackpawn.com/texts/pointinpoly/default.html
	// Change of basis but my matrix math isnt working for some reason
	bool intersectWithTriangle(const glm::vec3& rayAtPoint, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		/*
		auto b = p2 - p1;
		auto c = p3 - p1;
		auto n = glm::cross(b,c);
		glm::mat3 b_c_n(b,c,n);
		glm::vec3 t_u_v = glm::inverse(b_c_n) * rayAtPoint;
		*/

		auto v0 = p3 - p1;
		auto v1 = p2 - p1;
		auto v2 = rayAtPoint - p1;

		// Compute dot products
		auto dot00 = glm::dot(v0, v0);
		auto dot01 = glm::dot(v0, v1);
		auto dot02 = glm::dot(v0, v2);
		auto dot11 = glm::dot(v1, v1);
		auto dot12 = glm::dot(v1, v2);

		// Compute barycentric coordinates
		auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		if ((u >= 0 && u <= 1) && (v >= 0 && v <= 1) && (u + v <= 1)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool intersectWithPoint(const glm::vec3& rayAtPoint, const glm::vec3& point) {
		// Replace .001 with "epsilon"
		if ((rayAtPoint.x <= point.x + 0.005) && (rayAtPoint.x >= point.x - 0.005) &&
			(rayAtPoint.y <= point.y + 0.005) && (rayAtPoint.y >= point.y - 0.005) &&
			(rayAtPoint.z <= point.z + 0.005) && (rayAtPoint.z >= point.z - 0.005)
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
}