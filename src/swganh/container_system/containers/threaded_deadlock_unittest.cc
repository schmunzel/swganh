#include <gtest/gtest.h>

#include "sorted_container_component.h"
#include "slotted_container_component.h"
#include "universe_container_component.h"

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

TEST(DeadlockTests, Itensity1)
{
	unsigned int thread_count = 10;

	//Create the Container Chain
	std::list<std::shared_ptr<Entity>> containerChain;
	for(unsigned int i=0; i < thread_count; ++i)
	{
		std::shared_ptr<Entity> a = std::make_shared<Entity>();
		switch(i % 3)
		{
		case 0:
			a->AttachComponent(std::make_shared<universe_container_component>(16384, 256, 128, false));
			containerChain.push_back(a);
			break;
		case 1:
			a->AttachComponent(std::make_shared<sorted_container_component>(std::make_shared<FullPermissions>()));
			containerChain.push_back(a);
			break;
		case 2:
			a->AttachComponent(std::make_shared<slotted_container_component>(std::make_shared<FullPermissions>()));
			containerChain.push_back(a);
			break;
		}
	}

	//For Every Container we need to create a thread and an entity that manages it.
	//For each of these we will alternate their direction in the chain they must follow.

}

TEST(DeadlockTests, Itensity2)
{

}

TEST(DeadlockTests, Itensity3)
{

}

TEST(DeadlockTests, IfThisDoesntDeadlockNothingWillIntensity)
{
	
}

};