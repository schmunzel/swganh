#include <swganh/containers/container_component_interface.h>
#include <anh/component/entity.h>

using namespace swganh::containers;
using namespace swganh::containers::permissions;

//Setup the Null Container
std::shared_ptr<NullContainerPermissions> ContainerPermissionsInterface::NullComponent = std::make_shared<NullContainerPermissions>();