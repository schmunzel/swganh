#ifndef MOCK_PERMISSIONS_H
#define MOCK_PERMISSIONS_H

#include <gmock/gmock.h>
#include <api/container_permissions_interface.h>

namespace 
{

class MockPermissions : public anh::api::ContainerPermissionsInterface
{
public:
	
	MOCK_METHOD1(can_view, bool(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD2(can_insert, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD3(can_insert, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>, const std::set<anh::HashString>&));
	MOCK_METHOD2(can_remove, bool(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>));
	
	MOCK_METHOD1(inserted, void(std::shared_ptr<anh::component::Entity>));
	MOCK_METHOD1(removed, void(std::shared_ptr<anh::component::Entity>));

	MOCK_METHOD0(accepts_changes, bool());
	MOCK_METHOD2(permissions_grant_view, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_revoke_view, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_grant_insert, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_revoke_insert, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_grant_removal, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_revoke_removal, std::set<std::shared_ptr<anh::component::Entity>>(std::shared_ptr<anh::component::Entity>, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_grant_view, std::set<std::shared_ptr<anh::component::Entity>>(std::string, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_revoke_view, std::set<std::shared_ptr<anh::component::Entity>>(std::string, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_grant_insert, std::set<std::shared_ptr<anh::component::Entity>>(std::string, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_revoke_insert, std::set<std::shared_ptr<anh::component::Entity>>(std::string, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_grant_removal, std::set<std::shared_ptr<anh::component::Entity>>(std::string, const std::set<std::shared_ptr<anh::component::Entity>>&));
	MOCK_METHOD2(permissions_revoke_removal, std::set<std::shared_ptr<anh::component::Entity>>(std::string, const std::set<std::shared_ptr<anh::component::Entity>>&));

	MOCK_METHOD0(size, size_t());
	MOCK_METHOD0(capacity, size_t());
	MOCK_METHOD1(capacity, bool(size_t));

private:
};

};

#endif