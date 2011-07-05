#ifndef CONTAINER_PERMISSIONS_INTERFACE_H
#define CONTAINER_PERMISSIONS_INTERFACE_H

#include <memory>

namespace anh
{
namespace component
{
	class Entity;
};

namespace api
{
class ContainerPermissionsInterface
{
public:
	virtual bool can_view(std::shared_ptr<anh::component::Entity> who) = 0;
	virtual bool can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) = 0;
	virtual bool can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) = 0;
	
	virtual void inserted(std::shared_ptr<anh::component::Entity> what) = 0;
	virtual void removed(std::shared_ptr<anh::component::Entity> what) = 0;
	virtual size_t size() = 0;

	virtual size_t capacity() = 0;
	virtual bool capacity(size_t new_capacity) = 0;
};

class NullContainerPermission : public ContainerPermissionsInterface
{
public:
	virtual bool can_view(std::shared_ptr<anh::component::Entity> who);
	virtual bool can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what);
	virtual bool can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what);

	virtual void inserted(std::shared_ptr<anh::component::Entity> what) { }
	virtual void removed(std::shared_ptr<anh::component::Entity> what) { }
	virtual size_t size() { return 0; }

	virtual size_t capacity() { return 0; }
	virtual bool capacity(size_t new_capacity) { return false; }
};
};
};

#endif