#ifndef CONTAINER_PERMISSIONS_INTERFACE_H
#define CONTAINER_PERMISSIONS_INTERFACE_H

#include <anh/hash_string.h>
#include <anh/component/base_component.h>
#include <memory>
#include <set>

namespace anh
{
namespace component
{
	class Entity;
};
};

namespace swganh { namespace containers { namespace permissions {

class NullContainerPermissions;

class ContainerPermissionsInterface : public anh::component::BaseComponent
{
public:
	ContainerPermissionsInterface(const anh::component::ComponentType& type) 
		: anh::component::BaseComponent("ContainerPermissions", type) {}

	virtual bool can_view(std::shared_ptr<anh::component::Entity> who) = 0;
	virtual bool can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) = 0;
	virtual bool can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, const std::set<anh::HashString>& arrangement_to_use) = 0;
	virtual bool can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) = 0;
	
	virtual void inserted(std::shared_ptr<anh::component::Entity> what) = 0;
	virtual void removed(std::shared_ptr<anh::component::Entity> what) = 0;
	virtual size_t size() = 0;

	virtual bool accepts_changes() = 0;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_view(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_view(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_insert(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_insert(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_removal(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_removal(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;

	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_view(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_view(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_insert(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_insert(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_removal(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_removal(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) = 0 ;

	virtual size_t capacity() = 0;
	virtual bool capacity(size_t new_capacity) = 0;

	static std::shared_ptr<NullContainerPermissions> NullComponent;
};

class NullContainerPermissions : public ContainerPermissionsInterface
{
public:
	NullContainerPermissions() : ContainerPermissionsInterface("Null") {}

	virtual size_t size() { return 0; }

	virtual bool accepts_changes() { return false; }

	virtual bool can_view(std::shared_ptr<anh::component::Entity> who) { return false; }
	virtual bool can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return false; }
	virtual bool can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, const std::set<anh::HashString>& arrangement_to_use) { return false; }
	virtual bool can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return false; }

	virtual void inserted(std::shared_ptr<anh::component::Entity> what) { }
	virtual void removed(std::shared_ptr<anh::component::Entity> what) { }

	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_view(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_view(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_insert(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_insert(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_removal(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_removal(std::shared_ptr<anh::component::Entity> who, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }

	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_view(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_view(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_insert(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_insert(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_grant_removal(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }
	virtual std::set<std::shared_ptr<anh::component::Entity>> permissions_revoke_removal(std::string argument, const std::set<std::shared_ptr<anh::component::Entity>>& aware_entities) { return std::set<std::shared_ptr<anh::component::Entity>>(); }

	virtual size_t capacity() { return 0; }
	virtual bool capacity(size_t new_capacity) { return false; }
};
};
};
};

#endif