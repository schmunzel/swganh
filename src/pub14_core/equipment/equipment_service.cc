#include "equipment_service.h"

#include <memory>
#include "anh/logger.h"
#include <swganh/tre/resource_manager.h>
#include <swganh/tre/visitors/slots/slot_definition_visitor.h>
#include <swganh/object/object.h>

using namespace anh::service;
using namespace swganh::object;
using namespace swganh::tre;
using namespace swganh_core::equipment;

EquipmentService::EquipmentService(swganh::tre::ResourceManager* resourceManager)
{
	slot_definitions_ = resourceManager->GetResourceByName<SlotDefinitionVisitor>("abstract/slot/slot_definition/slot_definitions.iff");
}

anh::service::ServiceDescription EquipmentService::GetServiceDescription()
{
	ServiceDescription service_description(        
		"Equipment Service",
        "equipment",
        "0.1",
        "127.0.0.1", 
        0,
        0, 
        0);
	return service_description;
}

int32_t EquipmentService::GetSlotIdByName(std::string slot_name)
{
	return slot_definitions_->findSlotByName(slot_name);
}

std::string EquipmentService::GetSlotNameById(int32_t slot_id)
{
	return slot_definitions_->entry(slot_id).name;
}

void EquipmentService::ClearSlot(std::shared_ptr<Object> object, std::string slot_name)
{
	if (!object->ClearSlot(slot_definitions_->findSlotByName(slot_name)))
		LOG(warning) << "Could not find slot with name " << slot_name << " in object " << object->GetObjectId();
}
		
std::shared_ptr<Object> EquipmentService::GetEquippedObject(std::shared_ptr<Object> object, std::string slot_name)
{
	size_t slot_id = slot_definitions_->findSlotByName(slot_name);
	if (slot_id >= 0)
		return object->GetSlotObject(slot_id);
	else
		LOG(warning) << "Slot " << slot_name << " does not exist for object:" << object->GetObjectId();
	return nullptr;
}