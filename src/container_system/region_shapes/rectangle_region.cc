#include <container_system/region_shapes/rectangle_region.h>

using namespace container_system;

rectangle_region::rectangle_region(glm::vec3& center, float width, float height)
{
	float half_width = width/2;
	float half_height = height/2;

	verts_.push_back(glm::vec3(center.x - half_width, 0, center.z - half_height));
	verts_.push_back(glm::vec3(center.x - half_width, 0, center.z + half_height));
	verts_.push_back(glm::vec3(center.x + half_width, 0, center.z - half_height));
	verts_.push_back(glm::vec3(center.x + half_width, 0, center.z + half_height));
}

bool rectangle_region::within_extent(const glm::vec3& position)
{
	return position.x >= verts_[0].x && position.x <= verts_[1].x && position.z >= verts_[2].y && position.z >= verts_[0].y;
}