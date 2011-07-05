#ifndef SLOTTED_CONTAINER_H
#define SLOTTED_CONTAINER_H

#include <container_system/containers/sorted_container_component.h>
#include <api/components/slot_arrangement_component.h>
#include <boost/thread/shared_mutex.hpp>

namespace spatial_index
{
namespace container
{
class slotted_container_component : public sorted_container_component
{
	slotted_container_component(std::shared_ptr<anh::api::ContainerPermissionsInterface> permissions)
		: sorted_container_component(permissions, &anh::component::ComponentType("Slotted")) { }

	//View Methods
	virtual std::shared_ptr<anh::component::Entity> entity_in_slot(std::shared_ptr<anh::component::Entity> who, std::string slot_name);

	//Modification Methods
	virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion=false);
	virtual bool remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal=false);
	virtual bool transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<anh::api::components::ContainerComponentInterface> recv_container, bool force_insertion=false, bool force_removal=false);

	bool intrl_insert_(std::shared_ptr<anh::component::Entity> what);

private:
	
	void remove_intrl_(std::shared_ptr<anh::api::SlotArrangementComponent> slots);

	std::map<std::string, std::shared_ptr<anh::component::Entity>> slotted_entities_;
};
};
};

#endif