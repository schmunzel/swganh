#ifndef SLOT_ARRANGEMENT_INTERFACE_H
#define SLOT_ARRANGEMENT_INTERFACE_H

#include <anh/component/base_component.h>
#include <anh/hash_string.h>

#include <set>
#include <vector>

namespace swganh
{
namespace slots
{

class SlotArrangementComponent : public anh::component::BaseComponent
{
public:
	SlotArrangementComponent() : anh::component::BaseComponent("SlotArrangement", "") {}

	void add_arrangement(const std::set<anh::HashString>& slot) { slots_occupied_.push_back(slot); }
	void remove_arrangement(size_t position) { slots_occupied_.erase(slots_occupied_.begin() + position); }

	size_t arrangements() { return slots_occupied_.size(); }
	const std::set<anh::HashString>& arrangement(unsigned int arrangement_id) { return slots_occupied_[arrangement_id]; }

	static std::shared_ptr<SlotArrangementComponent> NullComponent;

private:

	std::vector<std::set<anh::HashString>> slots_occupied_;

};

};
};

#endif