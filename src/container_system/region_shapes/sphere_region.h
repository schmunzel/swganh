#ifndef SPHERE_REGION_H
#define SPHERE_REGION_H

#include <api/region_interface.h>
namespace container_system
{

class sphere_region : public container_system::region_interface
{
public:
	sphere_region(glm::vec3& center, float radius);

	virtual bool within_extent(const glm::vec3& position);
	virtual const std::vector<glm::vec3>& verts() {return verts_;}

private:
	std::vector<glm::vec3> verts_;

	glm::vec3 center_;
	float radius_squared_;
};

};

#endif