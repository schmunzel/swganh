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

class NullSlotArrangement;
class SlotArrangementInterface : public anh::component::BaseComponent
{
public:
	SlotArrangementInterface(const anh::component::ComponentType& type) : anh::component::BaseComponent("SlotArrangement", type) {}

	virtual void add_arrangement(const std::set<anh::HashString>& slot) = 0;
	virtual void remove_arrangement(size_t position) = 0;

	virtual size_t arrangements() = 0;
	virtual const std::set<anh::HashString>& arrangement(unsigned int arrangement_id) = 0;

	static std::shared_ptr<NullSlotArrangement> NullComponent;
};

class NullSlotArrangement : public SlotArrangementInterface
{
public:

	NullSlotArrangement() : SlotArrangementInterface("Null") {}

	virtual void add_arrangement(const std::set<anh::HashString>& slot) {}
	virtual void remove_arrangement(size_t position) {}

	virtual size_t arrangements() { return 0; }
	virtual const std::set<anh::HashString>& arrangement(unsigned int arrangement_id) {return slots_;}

private:
	std::set<anh::HashString> slots_;
};

};
};

#endif