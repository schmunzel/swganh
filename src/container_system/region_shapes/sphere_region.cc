#include <container_system/region_shapes/sphere_region.h>
#include <math.h>

using namespace container_system;

sphere_region::sphere_region(glm::vec3& center, float radius)
	: center_(center)
	, radius_squared_(radius*radius)
{
	verts_.push_back(glm::vec3(center.x - radius, center.y - radius, center.z - radius));		
	verts_.push_back(glm::vec3(center.x - radius, center.y - radius, center.z + radius));		
	verts_.push_back(glm::vec3(center.x + radius, center.y - radius, center.z - radius));		
	verts_.push_back(glm::vec3(center.x + radius, center.y - radius, center.z + radius));		

	verts_.push_back(glm::vec3(center.x - radius, center.y + radius, center.z - radius));		
	verts_.push_back(glm::vec3(center.x - radius, center.y + radius, center.z + radius));		
	verts_.push_back(glm::vec3(center.x + radius, center.y + radius, center.z - radius));		
	verts_.push_back(glm::vec3(center.x + radius, center.y + radius, center.z + radius));		
}

bool sphere_region::within_extent(const glm::vec3& position)
{
	return (pow(center_.x - position.x, 2.0f) + pow(center_.z - position.z, 2.0f) + pow(center_.y - position.y, 2.0f)) <= radius_squared_;
}