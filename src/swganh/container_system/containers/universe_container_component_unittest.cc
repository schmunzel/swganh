#include <gtest/gtest.h>

#include "universe_container_component.h"
#include "mock_container_component.h"
#include "../region_shapes/mock_region.h"
#include "permissions/mock_permissions.h"

#include <anh/component/entity.h>
#include <api/components/slot_arrangement_component.h>
#include <api/components/transform_component_interface.h>
#include <mod_anh_transform/transform_component.h>

using namespace anh::component;
using namespace anh::api::components;
using namespace container_system;
using namespace ::testing;

std::shared_ptr<anh::api::components::NullTransformComponent> anh::api::components::TransformComponentInterface::NullComponent = std::make_shared<anh::api::components::NullTransformComponent>();

namespace
{

std::shared_ptr<Entity> entity(glm::vec3& pos, glm::quat& rot, float speed)
{
	std::shared_ptr<Entity> res = std::make_shared<Entity>();
	res->AttachComponent(std::make_shared<transform::TransformComponent>(pos, rot, speed));
	return res;
}

TEST(UniverseContainerTests, ConfirmIrrelivantCalls)
{
	std::shared_ptr<Entity> uni = std::make_shared<Entity>();
	std::shared_ptr<universe_container_component> universe = std::make_shared<universe_container_component>(16384, 256, 128, false);
	uni->AttachComponent(universe);

	ASSERT_EQ(false, universe->empty());
	ASSERT_EQ(false, universe->full());
	ASSERT_EQ(0, universe->size());
	ASSERT_EQ(0, universe->capacity());
	ASSERT_EQ(false, universe->capacity(125));
	ASSERT_EQ(true, universe->has_entity(std::make_shared<anh::component::Entity>(), std::make_shared<anh::component::Entity>()));
	ASSERT_EQ(nullptr, universe->entity_in_slot(std::make_shared<anh::component::Entity>(),"head"));
}

TEST(UniverseContainerTests, InsertAndRemoveEntityAnd2DRegion)
{
	std::shared_ptr<Entity> uni = std::make_shared<Entity>();
	std::shared_ptr<universe_container_component> universe = std::make_shared<universe_container_component>(16384, 256, 128, false);
	uni->AttachComponent(universe);

	std::shared_ptr<mock_region> region_mock = std::make_shared<mock_region>();
	std::shared_ptr<Entity> e = entity(glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 0), 0);

	std::vector<glm::vec3> points;
	points.push_back(glm::vec3(-10, 0, -10));
	points.push_back(glm::vec3(-10, 0, 10));
	points.push_back(glm::vec3(10, 0, -10));
	points.push_back(glm::vec3(10, 0, 10));

	EXPECT_CALL(*region_mock, verts())
		.Times(2)
		.WillOnce(ReturnRef(points))
		.WillOnce(ReturnRef(points));

	EXPECT_CALL(*region_mock, within_extent(_))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(true));

	EXPECT_CALL(*region_mock, on_appear(_))
		.Times(1);

	EXPECT_CALL(*region_mock, on_disappear(_))
		.Times(1);

	universe->insert(nullptr, e, true);

	universe->insert(region_mock);
	universe->remove(region_mock);

	universe->remove(nullptr, e, true);
}

TEST(UniverseContainerTests, Handles2DStateUpdate)
{
	//This is the same first test but with some reordered calls.
	std::shared_ptr<Entity> uni = std::make_shared<Entity>();
	std::shared_ptr<universe_container_component> universe = std::make_shared<universe_container_component>(16384, 256, 128, false);
	uni->AttachComponent(universe);

	std::shared_ptr<mock_region> region_mock = std::make_shared<mock_region>();
	std::shared_ptr<Entity> e = entity(glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 0), 0);

	std::vector<glm::vec3> points;
	points.push_back(glm::vec3(-10, 0, -10));
	points.push_back(glm::vec3(-10, 0, 10));
	points.push_back(glm::vec3(10, 0, -10));
	points.push_back(glm::vec3(10, 0, 10));

	EXPECT_CALL(*region_mock, verts())
		.Times(2)
		.WillOnce(ReturnRef(points))
		.WillOnce(ReturnRef(points));

	EXPECT_CALL(*region_mock, within_extent(_))
		.Times(3)
		.WillOnce(Return(true))
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*region_mock, on_enter(_))
		.Times(1);

	EXPECT_CALL(*region_mock, on_exit(_))
		.Times(1);

	universe->insert(region_mock);
	universe->insert(nullptr, e, true);

	e->QueryInterface<TransformComponentInterface>("Transform")->move_forward(10.0f);
	e->QueryInterface<TransformComponentInterface>("Transform")->rotate_left(10.0f);
	
	universe->remove(nullptr, e, true);
	universe->remove(region_mock);
}

TEST(UniverseContainerTests, AwarenessTests)
{
	/*
	A
		//Near each other
		B 
		C

		//Outside Viewing Range
		D
	*/

	auto a = std::make_shared<Entity>();
	auto b = entity(glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 0), 0);
	auto c = entity(glm::vec3(10, 10, 10), glm::quat(0, 0, 0, 0), 0);
	auto d = entity(glm::vec3(-8000, -8000, -8000), glm::quat(0, 0, 0, 0), 0);

	auto a_container = std::make_shared<universe_container_component>(16384, 256, 128, false); 
	a->AttachComponent(a_container);
	
	auto b_container = std::make_shared<MockContainerComponent>(); 
	b->AttachComponent(b_container);

	auto c_container = std::make_shared<MockContainerComponent>(); 
	c->AttachComponent(c_container);
	
	auto d_container = std::make_shared<MockContainerComponent>(); 
	d->AttachComponent(d_container);

	EXPECT_CALL(*b_container, make_aware(_));
	EXPECT_CALL(*c_container, make_aware(_));
	EXPECT_CALL(*c_container, make_unaware(_));

	a_container->insert(nullptr, b, true);
	a_container->insert(nullptr, c, true);
	a_container->insert(nullptr, d, true);

	a_container->remove(nullptr, b, true);
	a_container->remove(nullptr, d, true);
	a_container->remove(nullptr, c, true);
}

TEST(UniverseContainerTests, HandlesContainedObjects)
{

}

TEST(UniverseContainerTests, HandlesTransferToOther)
{

}

TEST(UniverseContainerTests, HandlesTransferInto)
{

}

TEST(UniverseContainerTests, Region2DEnterAndExitNotifications)
{
	//This is the same first test but with some reordered calls.
	std::shared_ptr<Entity> uni = std::make_shared<Entity>();
	std::shared_ptr<universe_container_component> universe = std::make_shared<universe_container_component>(16384, 256, 128, false);
	uni->AttachComponent(universe);

	std::shared_ptr<mock_region> region_mock = std::make_shared<mock_region>();
	std::shared_ptr<Entity> e = entity(glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 0), 0);

	std::vector<glm::vec3> points;
	points.push_back(glm::vec3(-10, 0, -10));
	points.push_back(glm::vec3(-10, 0, 10));
	points.push_back(glm::vec3(10, 0, -10));
	points.push_back(glm::vec3(10, 0, 10));

	EXPECT_CALL(*region_mock, verts())
		.Times(2)
		.WillOnce(ReturnRef(points))
		.WillOnce(ReturnRef(points));

	EXPECT_CALL(*region_mock, within_extent(_))
		.Times(2)
		.WillOnce(Return(true))
		.WillOnce(Return(false));

	EXPECT_CALL(*region_mock, on_enter(_))
		.Times(1);

	EXPECT_CALL(*region_mock, on_exit(_))
		.Times(1);

	universe->insert(region_mock);

	universe->insert(nullptr, e, true);
	universe->remove(nullptr, e, true);
	
	universe->remove(region_mock);
}

};
