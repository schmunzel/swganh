#ifndef RECTANGULAR_PRISM_REGION_H
#define RECTANGULAR_PRISM_REGION_H

#include <api/region_interface.h>

namespace container_system
{
class rectangular_prism_region : public container_system::region_interface
{
public:
	rectangular_prism_region(glm::vec3& center, float length, float width, float height);

	virtual bool within_extent(const glm::vec3& position);
	virtual const std::vector<glm::vec3>& verts() {return verts_;}

private:
	std::vector<glm::vec3> verts_;
};

};
#endif