#include "rectangular_prism_region.h"

using namespace swganh::regions;

rectangular_prism_region::rectangular_prism_region(glm::vec3& center, float length, float width, float height)
{
	float half_width = width/2;
	float half_height = height/2;
	float half_length = length/2;

	//This section could be condensed a bit to make it faster if need be.'
	//It will most likely not be an issue because this code is only run on the creation of a region.
	verts_.push_back(glm::vec3(center.x - half_width, center.y - half_height, center.z - half_length));
	verts_.push_back(glm::vec3(center.x - half_width, center.y - half_height, center.z + half_length));
	verts_.push_back(glm::vec3(center.x + half_width, center.y - half_height, center.z - half_length));
	verts_.push_back(glm::vec3(center.x + half_width, center.y - half_height, center.z + half_length));

	verts_.push_back(glm::vec3(center.x - half_width, center.y + half_height, center.z - half_length));
	verts_.push_back(glm::vec3(center.x - half_width, center.y + half_height, center.z + half_length));
	verts_.push_back(glm::vec3(center.x + half_width, center.y + half_height, center.z - half_length));
	verts_.push_back(glm::vec3(center.x + half_width, center.y + half_height, center.z + half_length));
}

bool rectangular_prism_region::within_extent(const glm::vec3& position)
{
	return	position.x >= verts_[0].x && 
			position.x <= verts_[1].x && 
			position.z >= verts_[0].z && 
			position.z <= verts_[2].z &&
			position.y >= verts_[0].y &&
			position.y <= verts_[4].y;
}