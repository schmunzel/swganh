#ifndef RECTANGLE_REGION_H
#define RECTANGLE_REGION_H

#include <api/region_interface.h>

namespace container_system
{
class rectangle_region : public container_system::region_interface
{
public:
	rectangle_region(glm::vec3& center, float width, float height);

	virtual bool within_extent(const glm::vec3& position);
	virtual const std::vector<glm::vec3>& verts() {return verts_;}

	virtual void on_appear(std::shared_ptr<anh::component::Entity> object) {}
	virtual void on_enter(std::shared_ptr<anh::component::Entity> object) {}
	virtual void on_exit(std::shared_ptr<anh::component::Entity> object) {}
	virtual void on_disappear(std::shared_ptr<anh::component::Entity> object) {}

private:
	glm::vec3 top_left, top_right, bottom_left, bottom_right;
	std::vector<glm::vec3> verts_;
};

};
#endif