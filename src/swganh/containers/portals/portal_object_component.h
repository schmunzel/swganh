#ifndef PORTAL_OBJECT_COMPONENT_H
#define PORTAL_OBJECT_COMPONENT_H

#include <anh/component/base_component.h>
#include <anh/component/entity.h>
#include <glm/glm.hpp>

namespace anh
{
namespace api
{
namespace components
{
class NullPortalObject;
class PortalObjectComponentInterface : public anh::component::BaseComponent
{
public:
	PortalObjectComponentInterface(const anh::component::ComponentType& type) 
		: BaseComponent("PortalObject", type) {}

	virtual anh::component::EntityId detect_parent(anh::component::EntityId current, glm::vec3 pos1, glm::vec3 pos2) = 0;

	static std::shared_ptr<NullPortalObject> NullComponent;
};

class NullPortalObject : public PortalObjectComponentInterface
{
public:
	NullPortalObject()
		: PortalObjectComponentInterface("Null") {}

	virtual anh::component::EntityId latest_parent(glm::vec3 pos1, glm::vec3 pos2) { return entity()->id(); }
};

};
};
};

#endif