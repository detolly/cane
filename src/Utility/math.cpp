#include "math.h"

glm::vec3 clickray(double mouse_x, double mouse_y, double width, double height, const glm::mat4& projection, Camera& camera)
{
	double x = (2.0f * mouse_x) / width - 1.0f;
	double y = 1.0f - (2.0f * mouse_y) / height;
	double z = 1.0f;
	//dbgprint("screen coords: %f %f\n", x, y);
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
	auto temp = glm::normalize(glm::inverse(camera.view()) * ray_eye);
	glm::vec3 ray = glm::vec3(temp.x, temp.y, temp.z);
	return ray;
}

bool ray_intersects_triangle(const glm::vec3& ray_origin, const glm::vec3& ray_vector, const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2, glm::vec3& outIntersectionPoint)
{
	const double EPSILON = 0.0000001;
	glm::vec3 edge1, edge2, h, s, q;
	double a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = glm::cross(ray_vector, edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;    // This ray is parallel to this triangle.
	f = 1.0 / a;
	s = ray_origin - vertex0;
	u = f * glm::dot(s, h);
	if (u < 0.0 || u > 1.0)
		return false;
	q = glm::cross(s, edge1);
	v = f * glm::dot(ray_vector, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	double t = f * glm::dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		outIntersectionPoint = ray_origin + ray_vector * static_cast<float>(t);
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}
