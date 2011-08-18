#include "slot_arrangement_component.h"

using namespace swganh::slots;

std::shared_ptr<NullSlotArrangement> SlotArrangementInterface::NullComponent = std::make_shared<NullSlotArrangement>();