#include <api/components/container_component_interface.h>
#include <api/container_permissions_interface.h>
#include <anh/component/entity.h>

using namespace anh::api;
using namespace anh::api::components;

//Setup the Null Container
std::shared_ptr<NullContainerComponent> ContainerComponentInterface::NullComponent = std::make_shared<NullContainerComponent>();
std::shared_ptr<NullContainerPermission> NullContainerComponent::null_permissions = std::make_shared<NullContainerPermission>();

bool NullContainerPermission::can_view(std::shared_ptr<anh::component::Entity> who) { return false; }
bool NullContainerPermission::can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return false; }
bool NullContainerPermission::can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, const std::set<anh::HashString>& arrangement_to_use) { return false; }
bool NullContainerPermission::can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return false; }

void NullContainerPermission::inserted(std::shared_ptr<anh::component::Entity> what) { }
void NullContainerPermission::removed(std::shared_ptr<anh::component::Entity> what) { }

std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_grant_view(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_revoke_view(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_grant_insert(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_revoke_insert(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_grant_removal(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_revoke_removal(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }

std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_grant_view(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_revoke_view(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_grant_insert(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_revoke_insert(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_grant_removal(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
std::set<std::shared_ptr<anh::component::Entity>> NullContainerPermission::permissions_revoke_removal(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }