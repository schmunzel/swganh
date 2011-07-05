#ifndef CIRCLE_REGION_H
#define CIRCLE_REGION_H

#include <api/region_interface.h>
namespace container_system
{

class circle_region : public container_system::region_interface
{
public:
	circle_region(glm::vec3& center, float radius);

	virtual bool within_extent(const glm::vec3& position);
	virtual const std::vector<glm::vec3>& verts() {return verts_;}

	virtual void on_appear(std::shared_ptr<anh::component::Entity> object) {}
	virtual void on_enter(std::shared_ptr<anh::component::Entity> object) {}
	virtual void on_exit(std::shared_ptr<anh::component::Entity> object) {}
	virtual void on_disappear(std::shared_ptr<anh::component::Entity> object) {}

private:
	std::vector<glm::vec3> verts_;

	glm::vec3 center_;
	float radius_squared_;
};

};

#endif