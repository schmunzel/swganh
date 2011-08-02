#include "rectangle_region.h"

using namespace swganh::regions;

rectangle_region::rectangle_region(glm::vec3& center, float length, float width)
{
	float half_width = width/2;
	float half_length = length/2;

	verts_.push_back(glm::vec3(center.x - half_width, 0, center.z - half_length));
	verts_.push_back(glm::vec3(center.x - half_width, 0, center.z + half_length));
	verts_.push_back(glm::vec3(center.x + half_width, 0, center.z - half_length));
	verts_.push_back(glm::vec3(center.x + half_width, 0, center.z + half_length));
}

bool rectangle_region::within_extent(const glm::vec3& position)
{
	return	position.x >= verts_[0].x &&
			position.x <= verts_[1].x &&
			position.z >= verts_[0].z &&
			position.z <= verts_[2].z;
}