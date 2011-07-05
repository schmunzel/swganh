#ifndef SLOT_ARRANGEMENT_INTERFACE_H
#define SLOT_ARRANGEMENT_INTERFACE_H

#include <anh/component/base_component.h>

namespace anh
{
namespace api
{

class SlotArrangementComponent : public anh::component::BaseComponent
{
public:

	SlotArrangementComponent()
		: anh::component::BaseComponent("SlotArrangement", "") {}

	void add_slot(const std::string& slot) { slots_occupied_.insert(slot); }
	void remove_slot(const std::string& slot) { slots_occupied_.erase(slot); }
	bool has_slot(const std::string& slot) { return slots_occupied_.find(slot) != slots_occupied_.end(); }

	const std::set<std::string>& slots_occupied() { return slots_occupied_; }

	static std::shared_ptr<SlotArrangementComponent> NullComponent;
private:
	std::set<std::string> slots_occupied_;
};
};
};

#endif