#include <gtest/gtest.h>

#include "sorted_container_component.h"
#include "mock_container_component.h"
#include "permissions/mock_permissions.h"

#include <anh/component/entity.h>

using namespace anh::component;
using namespace anh::api::components;
using namespace container_system;
using namespace ::testing;

namespace
{

TEST(SortedContainerTests, InsertEntityDependsOnPermissions)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillRepeatedly(Return(false));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	//Do the calls
	EXPECT_EQ(true, sorted->insert(std::make_shared<Entity>(), std::make_shared<Entity>(), false));
	EXPECT_EQ(false, sorted->insert(std::make_shared<Entity>(), std::make_shared<Entity>(), false));
	EXPECT_EQ(true, sorted->insert(std::make_shared<Entity>(), std::make_shared<Entity>(), true));
}

TEST(SortedContainerTests, CannotInsertEntityViaSlotList)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Do the calls
	EXPECT_EQ(false, sorted->insert(std::make_shared<Entity>(), std::make_shared<Entity>(), 0, false));
	EXPECT_EQ(false, sorted->insert(std::make_shared<Entity>(), std::make_shared<Entity>(), 0, true));
}

TEST(SortedContainerTests, RemoveEntityDependsOnPermissions)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	sorted->insert(a, b, true);
	sorted->insert(b, a, true);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, can_remove(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillRepeatedly(Return(false));

	EXPECT_CALL(*permission_mock, removed(_))
		.Times(2);

	//Do the calls
	EXPECT_EQ(true, sorted->remove(a, b, false));
	EXPECT_EQ(false, sorted->remove(a, b, false));
	EXPECT_EQ(true, sorted->remove(b, a, true));
}

TEST(SortedContainerTests, TransferToTestsPermissionsOK)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	sorted->insert(a, a, true);
	sorted->insert(b, b, true);

	//Do the test
	EXPECT_CALL(*permission_mock, can_remove(_, _))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*mock_container, permissions_can_insert(_, _))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*mock_container, aware_entities(_))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	EXPECT_CALL(*mock_container, intrl_insert_(_, _))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, removed(_))
		.Times(1);

	ASSERT_EQ(true, sorted->transfer_to(a, b, mock_container, false, false));
}

TEST(SortedContainerTests, TransferToPermissionsBadRemove)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	sorted->insert(a, a, true);
	sorted->insert(b, b, true);

	//Do the test
	EXPECT_CALL(*permission_mock, can_remove(_, _))
		.Times(1)
		.WillOnce(Return(false));

	ASSERT_EQ(false, sorted->transfer_to(a, b, mock_container, false, false));
}

TEST(SortedContainerTests, TransferToPermissionsBadInsert)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	sorted->insert(a, a, true);
	sorted->insert(b, b, true);

	//Do the test
	EXPECT_CALL(*permission_mock, can_remove(_, _))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*mock_container, permissions_can_insert(_, _))
		.Times(1)
		.WillOnce(Return(false));

	ASSERT_EQ(false, sorted->transfer_to(a, b, mock_container, false, false));
}

TEST(SortedContainerTests, MakeAwareProveRecursive)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	std::shared_ptr<Entity> the_ent = std::make_shared<Entity>();
	the_ent->AttachComponent(mock_container);

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	sorted->insert(the_ent, the_ent, true);

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*mock_container, make_aware(_))
		.Times(1);

	sorted->make_aware(std::make_shared<Entity>());
}

TEST(SortedContainerTests, MakeAwareNonRecurseWhenFalsePermissions)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	std::shared_ptr<Entity> the_ent = std::make_shared<Entity>();
	the_ent->AttachComponent(mock_container);

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	sorted->insert(the_ent, the_ent, true);

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(1)
		.WillOnce(Return(false));

	EXPECT_CALL(*mock_container, make_aware(_))
		.Times(0);

	sorted->make_aware(std::make_shared<Entity>());
}

TEST(SortedContainerTests, MakeUnawareProveRecursive)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	std::shared_ptr<Entity> the_ent = std::make_shared<Entity>();
	the_ent->AttachComponent(mock_container);

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	sorted->insert(the_ent, the_ent, true);

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*mock_container, make_unaware(_))
		.Times(1);

	sorted->make_unaware(std::make_shared<Entity>());
}

TEST(SortedContainerTests, MakeUnawareNonRecurseWhenFalsePermissions)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	std::shared_ptr<Entity> the_ent = std::make_shared<Entity>();
	the_ent->AttachComponent(mock_container);

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	sorted->insert(the_ent, the_ent, true);

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(1)
		.WillOnce(Return(false));

	EXPECT_CALL(*mock_container, make_aware(_))
		.Times(0);

	sorted->make_aware(std::make_shared<Entity>());
}

TEST(SortedContainerTests, AwareEntitiesWorks)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<ContainerComponentInterface> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<MockContainerComponent> mock_container = std::make_shared<MockContainerComponent>();
	std::shared_ptr<Entity> the_ent = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_CALL(*mock_container, make_aware(_))
		.Times(0);

	sorted->make_aware(the_ent);

	std::set<std::shared_ptr<Entity>> entities = sorted->aware_entities(the_ent);

	EXPECT_EQ(1, entities.size());
	EXPECT_EQ(the_ent, *entities.begin());
}

TEST(SortedContainerTests, CanViewCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	//Do the calls
	EXPECT_EQ(true, sorted->permissions_can_view(a));
	EXPECT_EQ(false, sorted->permissions_can_view(b));
}

TEST(SortedContainerTests, CanInsertCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	//Do the calls
	EXPECT_EQ(true, sorted->permissions_can_insert(a, b));
	EXPECT_EQ(false, sorted->permissions_can_insert(b, a));
}

TEST(SortedContainerTests, CanInsertWithArrangementDoesntCallSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();
	std::set<anh::HashString> setA;

	//Setup Expectations
	EXPECT_CALL(*permission_mock, can_insert(_, _, _))
		.Times(0);

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_can_insert(b, a, setA));
}

TEST(SortedContainerTests, CanRemoveCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, can_remove(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	//Do the calls
	EXPECT_EQ(true, sorted->permissions_can_remove(a, b));
	EXPECT_EQ(false, sorted->permissions_can_remove(b, a));
}

TEST(SortedContainerTests, CanGrantViewCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_grant_view(A<std::shared_ptr<Entity>>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_grant_view(a));
	EXPECT_EQ(true, sorted->permissions_grant_view(b));
}

TEST(SortedContainerTests, CanRevokeViewCallsSubObject)
{
//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_revoke_view(A<std::shared_ptr<Entity>>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_revoke_view(a));
	EXPECT_EQ(true, sorted->permissions_revoke_view(b));
}

TEST(SortedContainerTests, CanGrantInsertCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_grant_insert(A<std::shared_ptr<Entity>>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_grant_insert(a));
	EXPECT_EQ(true, sorted->permissions_grant_insert(b));
}

TEST(SortedContainerTests, CanRevokeInsertCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_revoke_insert(A<std::shared_ptr<Entity>>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_revoke_insert(a));
	EXPECT_EQ(true, sorted->permissions_revoke_insert(b));
}

TEST(SortedContainerTests, CanGrantRemoveCallsSubObject)
{
//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_grant_removal(A<std::shared_ptr<Entity>>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_grant_removal(a));
	EXPECT_EQ(true, sorted->permissions_grant_removal(b));
}

TEST(SortedContainerTests, CanRevokeRemoveCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_revoke_removal(A<std::shared_ptr<Entity>>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_revoke_removal(a));
	EXPECT_EQ(true, sorted->permissions_revoke_removal(b));
}

TEST(SortedContainerTests, CanGrantViewWithArgCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_grant_view(A<std::string>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_grant_view("Apathy"));
	EXPECT_EQ(true, sorted->permissions_grant_view("Kronos"));
}

TEST(SortedContainerTests, CanRevokeViewWithArgCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_revoke_view(A<std::string>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_revoke_view("Apathy"));
	EXPECT_EQ(true, sorted->permissions_revoke_view("Kronos"));
}

TEST(SortedContainerTests, CanGrantInsertWithArgCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_grant_insert(A<std::string>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_grant_insert("Apathy"));
	EXPECT_EQ(true, sorted->permissions_grant_insert("Kronos"));
}

TEST(SortedContainerTests, CanRevokeInsertWithArgCallsSubObject)
{
//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_revoke_insert(A<std::string>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_revoke_insert("Apathy"));
	EXPECT_EQ(true, sorted->permissions_revoke_insert("Kronos"));
}

TEST(SortedContainerTests, CanGrantRemoveWithArgCallsSubObject)
{
//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_grant_removal(A<std::string>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_grant_removal("Apathy"));
	EXPECT_EQ(true, sorted->permissions_grant_removal("Kronos"));
}

TEST(SortedContainerTests, CanRevokeRemoveWithArgCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	//Setup Expectations
	EXPECT_CALL(*permission_mock, accepts_changes())
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, permissions_revoke_removal(A<std::string>(), _))
		.Times(1)
		.WillOnce(Return(std::set<std::shared_ptr<Entity>>()));

	//Do the calls
	EXPECT_EQ(false, sorted->permissions_revoke_removal("Apathy"));
	EXPECT_EQ(true, sorted->permissions_revoke_removal("Kronos"));
}

TEST(SortedContainerTests, EmptyCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	EXPECT_CALL(*permission_mock, size())
		.Times(2)
		.WillOnce(Return(1))
		.WillOnce(Return(0));

	EXPECT_EQ(false, sorted->empty());
	EXPECT_EQ(true, sorted->empty());
}

TEST(SortedContainerTests, FullCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	EXPECT_CALL(*permission_mock, capacity())
		.Times(2)
		.WillOnce(Return(0))
		.WillOnce(Return(1));

	EXPECT_CALL(*permission_mock, size())
		.Times(2)
		.WillOnce(Return(0))
		.WillOnce(Return(0));

	EXPECT_EQ(true, sorted->full());
	EXPECT_EQ(false, sorted->full());
}

TEST(SortedContainerTests, SizeCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	EXPECT_CALL(*permission_mock, size())
		.Times(2)
		.WillOnce(Return(0))
		.WillOnce(Return(102));

	EXPECT_EQ(0, sorted->size());
	EXPECT_EQ(102, sorted->size());
}

TEST(SortedContainerTests, CapacityCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	EXPECT_CALL(*permission_mock, capacity())
		.Times(2)
		.WillOnce(Return(0))
		.WillOnce(Return(102));

	EXPECT_EQ(0, sorted->capacity());
	EXPECT_EQ(102, sorted->capacity());
}

TEST(SortedContainerTests, SetCapacityCallsSubObject)
{
	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	EXPECT_CALL(*permission_mock, capacity(_))
		.Times(2)
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_EQ(false, sorted->capacity(12));
	EXPECT_EQ(true, sorted->capacity(31));
}

TEST(SortedContainerTests, HasEntityWorks)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);
	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	sorted->insert(a, a, true);
	sorted->insert(b, b, true);

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_EQ(true, sorted->has_entity(a, b));
	EXPECT_EQ(false, sorted->has_entity(b, a));
}

TEST(SortedContainerTests, ContainedObjectsWorks)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	std::shared_ptr<Entity> a = std::make_shared<Entity>();
	std::shared_ptr<Entity> b = std::make_shared<Entity>();

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	unsigned int i = 0;
	sorted->contained_objects(a, [&] (std::shared_ptr<Entity> who, std::shared_ptr<Entity> what) {
		++i;
	});

	sorted->contained_objects(a, [] (std::shared_ptr<Entity> who, std::shared_ptr<Entity> what) {
		ASSERT_EQ(0, -1) << "`Who` can see `what` when he shouldnt!";
	});
}

TEST(SortedContainerTests, EntityInSlotReturnsNull)
{
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<sorted_container_component> sorted = std::make_shared<sorted_container_component>(permission_mock);

	EXPECT_EQ(nullptr, sorted->entity_in_slot(std::shared_ptr<Entity>(), "ship_pilot"));
}

};
