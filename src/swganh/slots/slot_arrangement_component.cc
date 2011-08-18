#include "slot_arrangement_component.h"

using namespace swganh::slots;

void SlotArrangementComponent::add_arrangement(const std::set<anh::HashString>& slot)
{
	slots_.push_back(slot);
}

void SlotArrangementComponent::remove_arrangement(size_t position)
{
	slots_.erase(slots_.begin()+position);
}

size_t SlotArrangementComponent::arrangements()
{
	return slots_.size();
}

const std::set<anh::HashString>& SlotArrangementComponent::arrangement(unsigned int arrangement_id)
{
	return slots_[arrangement_id];
}