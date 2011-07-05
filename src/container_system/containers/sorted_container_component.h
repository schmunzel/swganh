#ifndef SORTED_CONTAINER_H
#define SORTED_CONTAINER_H

#include <api/components/container_component_interface.h>
#include <boost/thread/shared_mutex.hpp>

namespace spatial_index
{
namespace container
{

class sorted_container_component : public anh::api::components::ContainerComponentInterface
{
public:
	sorted_container_component(std::shared_ptr<anh::api::ContainerPermissionsInterface> permissions, anh::component::ComponentType* type = nullptr)
		: ContainerComponentInterface((type) ? *type : "Sorted")
		, permissions_(permissions) {}

	//View Methods
	virtual bool has_entity(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what);
	virtual bool sorted_container_component::contained_objects(std::shared_ptr<anh::component::Entity> who, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth=0, bool top_down=true);
	virtual std::shared_ptr<anh::component::Entity> entity_in_slot(std::shared_ptr<anh::component::Entity> who,std::string slot_name) { return nullptr; }

	//Modification Methods
	virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion=false);
	virtual bool remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal);
	virtual bool transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<anh::api::components::ContainerComponentInterface> recv_container, bool force_insertion=false, bool for_removal=false);
	
	//Notification Methods
	virtual void make_aware(std::shared_ptr<anh::component::Entity> what);
	virtual void state_update(std::shared_ptr<anh::component::Entity> what, glm::vec3& old);
	virtual void make_unaware(std::shared_ptr<anh::component::Entity> what);
	virtual std::set<std::shared_ptr<anh::component::Entity>> aware_entities(std::shared_ptr<anh::component::Entity> caller_hint);

	//Permission Methods
	virtual std::shared_ptr<anh::api::ContainerPermissionsInterface> permissions();
	virtual bool permissions(std::shared_ptr<anh::api::ContainerPermissionsInterface> new_permissions);

	//Size/Capacity Methods
	virtual bool empty();
	virtual bool full();

	virtual size_t size();
	virtual size_t capacity();
	virtual bool capacity(size_t new_capacity);

protected:
	boost::shared_mutex intrl_lock_;

	std::set<std::shared_ptr<anh::component::Entity>> aware_entities_;

	std::set<std::shared_ptr<anh::component::Entity>> contained_objects_;
	std::shared_ptr<anh::api::ContainerPermissionsInterface> permissions_;
};

};
};


#endif