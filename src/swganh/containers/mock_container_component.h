#ifndef MOCK_CONTAINER_COMPONENT_H
#define MOCK_CONTAINER_COMPONENT_H

#include <gmock/gmock.h>
#include "container_component_interface.h"

namespace
{

class MockContainerComponent : public swganh::containers::ContainerComponentInterface
{
public:

	MockContainerComponent() : ContainerComponentInterface("Mock") {}

	//View Methods
	MOCK_METHOD2(has_entity, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD5(contained_objects, bool(std::shared_ptr<anh::component::Entity>, bool causes_populate, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)>, size_t, bool));
	MOCK_METHOD2(entity_in_slot, std::shared_ptr<anh::component::Entity>(std::shared_ptr<anh::component::Entity>,std::string));

	//Modification Methods
	MOCK_METHOD3(insert, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>, bool));
	MOCK_METHOD4(insert, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>, unsigned int, bool));
	MOCK_METHOD3(remove, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>, bool));
	MOCK_METHOD5(transfer_to, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>, std::shared_ptr<swganh::containers::ContainerComponentInterface>, bool, bool));

	//Awareness Methods
	MOCK_METHOD1(make_aware, void(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(make_unaware, void(std::shared_ptr<anh::component::Entity>));

	MOCK_METHOD4(state_update, void(std::shared_ptr<anh::component::Entity>, const glm::vec3& oldpos, const glm::vec3& newpos, const glm::quat& rot));
	
	MOCK_METHOD1(aware_entities, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>));

	//Permission Methods
	MOCK_METHOD1(permissions_can_view, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD2(permissions_can_insert, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD3(permissions_can_insert, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>, const std::set<anh::HashString>&));
	MOCK_METHOD2(permissions_can_remove, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>));

	MOCK_METHOD1(permissions_grant_view, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(permissions_revoke_view, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(permissions_grant_insert, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(permissions_revoke_insert, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(permissions_grant_removal, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(permissions_revoke_removal, bool(std::shared_ptr<anh::component::Entity>));

	MOCK_METHOD1(permissions_grant_view, bool(std::string));
	MOCK_METHOD1(permissions_revoke_view, bool(std::string));
	MOCK_METHOD1(permissions_grant_insert, bool(std::string));
	MOCK_METHOD1(permissions_revoke_insert, bool(std::string));
	MOCK_METHOD1(permissions_grant_removal, bool(std::string));
	MOCK_METHOD1(permissions_revoke_removal, bool(std::string));

	MOCK_METHOD0(empty, bool());
	MOCK_METHOD0(full, bool());

	MOCK_METHOD0(size, size_t());
	MOCK_METHOD0(capacity, size_t());
	MOCK_METHOD1(capacity, bool(size_t));

	MOCK_METHOD0(collect_garbage, bool());
	MOCK_METHOD0(is_populated, bool());

	MOCK_METHOD0(persist, void());
	MOCK_METHOD0(populate, void());

	MOCK_METHOD2(intrl_insert_, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<ContainerComponentInterface>));
};

};

#endif