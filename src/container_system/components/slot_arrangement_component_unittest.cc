#include <gtest/gtest.h>
#include <anh/component/entity.h>
#include <api/components/slot_arrangement_component.h>

using namespace anh::component;
using namespace anh::api::components;

namespace
{

TEST(SlotArrangementTests, NullSlotArrangementIsEmpty)
{
	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	std::shared_ptr<SlotArrangementComponent> arr = e->QueryInterface<SlotArrangementComponent>("SlotArrangement");

	EXPECT_EQ(SlotArrangementComponent::NullComponent, arr) << "We do not get the NullComponent as we should.";
	EXPECT_EQ(0, arr->arrangements()) << "NullComponent does not start out empty.";
}

TEST(SlotArrangementTests, AddArrangement)
{
	std::shared_ptr<Entity> e = std::make_shared<Entity>();

	e->AttachComponent(std::make_shared<SlotArrangementComponent>());
	std::shared_ptr<SlotArrangementComponent> arr = e->QueryInterface<SlotArrangementComponent>("SlotArrangement");

	std::set<anh::HashString> names;
	names.insert(anh::HashString("Powerking"));
	names.insert(anh::HashString("Apathy"));
	names.insert(anh::HashString("Kronos"));

	arr->add_arrangement(names);

	EXPECT_EQ(1, arr->arrangements());
}

TEST(SlotArrangementTests, RemoveArrangement)
{
	std::shared_ptr<Entity> e = std::make_shared<Entity>();

	e->AttachComponent(std::make_shared<SlotArrangementComponent>());
	std::shared_ptr<SlotArrangementComponent> arr = e->QueryInterface<SlotArrangementComponent>("SlotArrangement");

	std::set<anh::HashString> names;
	names.insert(anh::HashString("Powerking"));
	names.insert(anh::HashString("Apathy"));
	names.insert(anh::HashString("Kronos"));

	arr->add_arrangement(names);

	EXPECT_EQ(1, arr->arrangements());

	arr->remove_arrangement(0);

	EXPECT_EQ(0, arr->arrangements());
}

TEST(SlotArrangementTests, GetAnArrangementById)
{
	std::shared_ptr<Entity> e = std::make_shared<Entity>();

	e->AttachComponent(std::make_shared<SlotArrangementComponent>());
	std::shared_ptr<SlotArrangementComponent> arr = e->QueryInterface<SlotArrangementComponent>("SlotArrangement");

	std::set<anh::HashString> names;
	names.insert(anh::HashString("Powerking"));
	names.insert(anh::HashString("Apathy"));
	names.insert(anh::HashString("Kronos"));

	arr->add_arrangement(names);

	EXPECT_EQ(1, arr->arrangements());

	std::set<anh::HashString> name = arr->arrangement(0);
	EXPECT_EQ(true, name.find(anh::HashString("Powerking")) != name.end());
	EXPECT_EQ(true, name.find(anh::HashString("Apathy")) != name.end());
	EXPECT_EQ(true, name.find(anh::HashString("Kronos")) != name.end());

}

};
