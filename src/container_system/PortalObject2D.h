#ifndef PORTAL_OBJECT_2D_H
#define PORTAL_OBJECT_2D_H

#include <api/components/portal_object_component.h>

namespace spatial_index
{
class PortalObject2D : public anh::api::components::PortalObjectComponentInterface
{
public:
	virtual anh::component::EntityId detect_parent(anh::component::EntityId current, glm::vec3 pos1, glm::vec3 pos2);
private:
	bool intersects_triangle_(glm::vec3 v1, glm::vec3 v2, glm::vec3 va, glm::vec3 vb, glm::vec3 vc);
	bool same_side_(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b);
};
};


#endif