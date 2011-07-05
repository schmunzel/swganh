#include <container_system/region_shapes/circle_region.h>
#include <math.h>

using namespace container_system;

circle_region::circle_region(glm::vec3& center, float radius)
	: center_(center)
	, radius_squared_(radius*radius)
{
	float half_radius = radius/2;
	verts_.push_back(glm::vec3(center.x - half_radius, 0, center.z - half_radius));		
	verts_.push_back(glm::vec3(center.x - half_radius, 0, center.z + half_radius));		
	verts_.push_back(glm::vec3(center.x + half_radius, 0, center.z - half_radius));		
	verts_.push_back(glm::vec3(center.x + half_radius, 0, center.z + half_radius));		
}

bool circle_region::within_extent(const glm::vec3& position)
{
	return (pow(center_.x - position.x, 2.0f) + pow(center_.z - position.z, 2.0f)) <= radius_squared_;
}