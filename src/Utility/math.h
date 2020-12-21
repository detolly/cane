#pragma once

#include <Renderer/Camera.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>

glm::vec3 clickray(double mouse_x, double mouse_y, double width, double height, const glm::mat4& projection, Camera& camera);

/* https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm thanks wikipedia<3*/
bool ray_intersects_triangle(const glm::vec3& ray_origin,
	const glm::vec3& ray_vector,
	const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2,
	glm::vec3& outIntersectionPoint);