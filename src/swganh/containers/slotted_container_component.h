#ifndef SLOTTED_CONTAINER_H
#define SLOTTED_CONTAINER_H

#include "sorted_container_component.h"
#include <boost/thread/shared_mutex.hpp>

namespace swganh
{
namespace slots
{
class SlotArrangementComponent;
};

namespace containers
{
class slotted_container_component : public sorted_container_component
{
public:
	class slot
	{
	public:
		slot(unsigned int id) : arrangement_used(0), slot_id(id), entity(nullptr) {}
		
		size_t slot_id; //Used to tell the client where the Entity is slotted
		std::shared_ptr<anh::component::Entity> entity; //Stores the entity
		size_t arrangement_used; //Aids removal of the inserted entity
	};

	slotted_container_component() : sorted_container_component("Slotted") {}

	//View Methods
	virtual std::shared_ptr<anh::component::Entity> entity_in_slot(std::shared_ptr<anh::component::Entity> who, std::string slot_name);

	//Modification Methods
	virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion);
	virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, unsigned int id, bool force_insertion);
	virtual bool remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal);
	virtual bool transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<swganh::containers::ContainerComponentInterface> recv_container, bool force_insertion, bool force_removal);

	//Permission Methods
	virtual bool intrl_insert_(std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> old_container);

	virtual void load_descriptor_(const std::set<anh::HashString>& descriptor)
	{
		size_t i = 0;
		std::for_each(descriptor.begin(), descriptor.end(), [&, this] (const anh::HashString& h) {
			slotted_entities_.insert(std::make_pair<anh::HashString, slotted_container_component::slot>(h, slotted_container_component::slot(i)));
		});
	}

private:

	bool remove_intrl_(const std::set<anh::HashString>& slots);

	std::map<anh::HashString, slot> slotted_entities_;

};
};
};

#endif