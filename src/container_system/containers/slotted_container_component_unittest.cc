#include <gtest/gtest.h>
#include "slotted_container_component.h"
#include "mock_container_component.h"
#include "permissions/mock_permissions.h"

#include <anh/component/entity.h>
#include <api/components/slot_arrangement_component.h>

using namespace anh::component;
using namespace anh::api::components;
using namespace container_system;
using namespace ::testing;

namespace
{

TEST(SlottedContainerTests, BasicSingleInsert)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("head");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	EXPECT_EQ(true, slotted->insert(e, e, false));
	EXPECT_EQ(false, slotted->insert(e, e, false));
}

TEST(SlottedContainerTests, BasicSingleInsertSpecific)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("head");

	std::set<anh::HashString> arrangement_set_A_noise1;
	arrangement_set_A_noise1.insert("poon");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A_noise1);
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	EXPECT_EQ(true, slotted->insert(e, e, 1, false));
	EXPECT_EQ(false, slotted->insert(e, e, 1, false));
}

TEST(SlottedContainerTests, BadBasicSingleInsert)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("foot");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(0);

	EXPECT_EQ(false, slotted->insert(e, e, false));
	EXPECT_EQ(false, slotted->insert(e, e, false));
}

TEST(SlottedContainerTests, DescriptorHasMoreSlots)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("head");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");
	descriptor_A.push_front("chest");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	EXPECT_EQ(true, slotted->insert(e, e, false));
	EXPECT_EQ(false, slotted->insert(e, e, false));
}

TEST(SlottedContainerTests, ArrangementHasMoreSlots)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("foot");
	arrangement_set_A.insert("leg_l");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(0);

	EXPECT_EQ(false, slotted->insert(e, e, false));
	EXPECT_EQ(false, slotted->insert(e, e, false));
}

TEST(SlottedContainerTests, SameArrangementReplacesOne)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("head");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<Entity> f = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);
	f->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(2);

	EXPECT_CALL(*permission_mock, removed(_))
		.Times(1);

	EXPECT_EQ(true, slotted->insert(e, e, false));
	EXPECT_EQ(true, slotted->insert(f, f, false));
}

TEST(SlottedContainerTests, SameArrangementReplacesMany)
{
	std::set<anh::HashString> arrangement_A;
	arrangement_A.insert("head");
	
	std::set<anh::HashString> arrangement_B;	
	arrangement_B.insert("chest");

	std::set<anh::HashString> arrangement_AB;
	arrangement_AB.insert("head");
	arrangement_AB.insert("chest");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");
	descriptor_A.push_front("chest");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	sac->add_arrangement(arrangement_A);
	e->AttachComponent(sac);
	
	std::shared_ptr<Entity> f = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac1 = std::make_shared<SlotArrangementComponent>();
	sac1->add_arrangement(arrangement_B);
	f->AttachComponent(sac1);

	std::shared_ptr<Entity> g = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac2 = std::make_shared<SlotArrangementComponent>();
	sac2->add_arrangement(arrangement_AB);
	g->AttachComponent(sac2);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(3)
		.WillOnce(Return(true))
		.WillOnce(Return(true))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(3);

	EXPECT_CALL(*permission_mock, removed(_))
		.Times(2);

	EXPECT_EQ(true, slotted->insert(e, e, false));
	EXPECT_EQ(true, slotted->insert(f, f, false));
	EXPECT_EQ(true, slotted->insert(g, g, false));
}

TEST(SlottedContainerTests, SameArrangementReplacesManyWithNoise)
{
	std::set<anh::HashString> arrangement_A;
	arrangement_A.insert("head");

	std::set<anh::HashString> arrangement_A_noise1;
	arrangement_A_noise1.insert("foot");
	
	std::set<anh::HashString> arrangement_B;	
	arrangement_B.insert("chest");

	std::set<anh::HashString> arrangement_B_noise1;	
	arrangement_B_noise1.insert("arm_l");

	std::set<anh::HashString> arrangement_B_noise2;	
	arrangement_B_noise2.insert("arm_r");

	std::set<anh::HashString> arrangement_AB;
	arrangement_AB.insert("head");
	arrangement_AB.insert("chest");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");
	descriptor_A.push_front("chest");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	sac->add_arrangement(arrangement_A_noise1);
	sac->add_arrangement(arrangement_A);
	e->AttachComponent(sac);
	
	std::shared_ptr<Entity> f = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac1 = std::make_shared<SlotArrangementComponent>();
	sac->add_arrangement(arrangement_B_noise1);
	sac1->add_arrangement(arrangement_B);
	sac->add_arrangement(arrangement_B_noise2);
	f->AttachComponent(sac1);

	std::shared_ptr<Entity> g = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac2 = std::make_shared<SlotArrangementComponent>();
	sac2->add_arrangement(arrangement_AB);
	g->AttachComponent(sac2);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(3)
		.WillOnce(Return(true))
		.WillOnce(Return(true))
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(3);

	EXPECT_CALL(*permission_mock, removed(_))
		.Times(2);

	EXPECT_EQ(true, slotted->insert(e, e, false));
	EXPECT_EQ(true, slotted->insert(f, f, false));
	EXPECT_EQ(true, slotted->insert(g, g, false));
}

TEST(SlottedContainerTests, EntityInSlot)
{
	std::set<anh::HashString> arrangement_set_A;
	arrangement_set_A.insert("head");

	std::list<anh::HashString> descriptor_A;
	descriptor_A.push_front("head");

	//Create the Permissions MockObject and Container
	std::shared_ptr<::MockPermissions> permission_mock = std::make_shared<::MockPermissions>();
	std::shared_ptr<slotted_container_component> slotted = std::make_shared<slotted_container_component>(permission_mock);
	
	slotted->add_descriptor(descriptor_A);

	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> sac = std::make_shared<SlotArrangementComponent>();
	
	sac->add_arrangement(arrangement_set_A);
	e->AttachComponent(sac);

	EXPECT_CALL(*permission_mock, can_insert(_, _))
		.Times(2)
		.WillOnce(Return(true));

	EXPECT_CALL(*permission_mock, inserted(_))
		.Times(1);

	EXPECT_EQ(true, slotted->insert(e, e, false));

	EXPECT_CALL(*permission_mock, can_view(_))
		.Times(1)
		.WillOnce(Return(true));

	EXPECT_EQ(e, slotted->entity_in_slot(e, "head"));
}

};
