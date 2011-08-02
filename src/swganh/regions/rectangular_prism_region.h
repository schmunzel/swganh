#ifndef RECTANGULAR_PRISM_REGION_H
#define RECTANGULAR_PRISM_REGION_H

#include "region_interface.h"

namespace swganh
{
namespace regions
{
class rectangular_prism_region : public swganh::regions::RegionInterface
{
public:
	rectangular_prism_region(glm::vec3& center, float length, float width, float height);

	virtual bool within_extent(const glm::vec3& position);
	virtual const std::vector<glm::vec3>& verts() {return verts_;}

private:
	std::vector<glm::vec3> verts_;
};

};
};
#endif