#pragma once
#include <glm/glm.hpp>

class Ray {
	public:
		Ray();
		Ray(float norm_x, float norm_y, glm::vec3 origin, glm::mat4 proj, glm::mat4 view);

		Ray& operator=(const Ray& c) 
		{
			// check for self-assignment
			if (&c == this)
				return *this;
			_origin = c._origin;
			_direction = c._direction;

			return *this;
		}


		void reset(float norm_x, float norm_y, glm::vec3 origin, glm::mat4 proj, glm::mat4 view);

		// Returns < 0 if no intersection, otherwise it returns the intersection distance
		bool intersectWithPlane(const glm::vec3& plane_normal, const glm::vec3& plane_offset, float& ray_magnitude) const;
		bool intersectWithPoint(const glm::vec3& point, float ray_magnitude) const;
		bool intersectWithPoint(const glm::vec3& rayAtPoint, const glm::vec3& point) const;
		bool intersectWithTriangle(const glm::vec3& V0, const glm::vec3& V1, const glm::vec3& V2) const;

		glm::vec3 atPoint(float t) const;

		// direction getters
		glm::vec3 direction() const { return _direction; }
		float d_x() const { return _direction.x; }
		float d_y() const { return _direction.y; }
		float d_z() const { return _direction.z; }

		// origin getters
		glm::vec3 origin() const { return _origin; }
		float o_x() const { return _origin.x; }
		float o_y() const { return _origin.y; }
		float o_z() const { return _origin.z; }

	private:
		glm::vec3 _origin;
		glm::vec3 _direction;
};

float distance(const Ray& a, const Ray& b);
