#include <container_system/containers/slotted_container_component.h>
#include <anh/component/entity.h>

#include <boost/thread/locks.hpp>

using namespace spatial_index::container;
using namespace anh::component;
using namespace anh::api::components;
using namespace anh::api;

bool slotted_container_component::intrl_insert_(std::shared_ptr<Entity> what)
{
	std::shared_ptr<SlotArrangementComponent> slots = what->QueryInterface<SlotArrangementComponent>("SlotArrangement");
	boost::unique_lock<boost::shared_mutex> lock(intrl_lock_);

	auto end = slots->slots_occupied().end();
	for(auto itr = slots->slots_occupied().begin(); itr != end; ++itr)
	{
		auto map_itr = slotted_entities_.lower_bound(*itr);
		if(map_itr != slotted_entities_.end() && !(slotted_entities_.key_comp()(*itr, map_itr->first)))
		{
			remove_intrl_(map_itr->second->QueryInterface<anh::api::SlotArrangementComponent>("SlotArrangement"));
			slotted_entities_.insert(std::make_pair<std::string, std::shared_ptr<Entity>>(*itr, what));
			permissions_->inserted(what);
		}
		else
		{
			slotted_entities_.insert(map_itr, std::map<std::string, std::shared_ptr<Entity>>::value_type(*itr, what));
			permissions_->inserted(what);
		}
		contained_objects_.insert(what);
	}
	return true;
}

void slotted_container_component::remove_intrl_(std::shared_ptr<SlotArrangementComponent> slots)
{
	auto end = slots->slots_occupied().end();
	for(auto itr = slots->slots_occupied().begin(); itr != end; ++itr)
	{
		auto dead_itr = slotted_entities_.find(*itr);
		if(dead_itr != slotted_entities_.end())
		{
			permissions_->removed(dead_itr->second);
			contained_objects_.erase(dead_itr->second);
			slotted_entities_.erase(dead_itr);
		}
	}
}

std::shared_ptr<Entity> slotted_container_component::entity_in_slot(std::shared_ptr<Entity> who, std::string slot_name)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(who))
	{
		auto itr = slotted_entities_.find(slot_name);
		if(itr != slotted_entities_.end())
		{
			return itr->second;
		}
	}
	return nullptr;
}

bool slotted_container_component::insert(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_insertion)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_insert(who, what) || force_insertion)
	{
		{ //Scope to allow the unique lock to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

			std::shared_ptr<SlotArrangementComponent> slots = what->QueryInterface<SlotArrangementComponent>("SlotArrangement");
			boost::unique_lock<boost::shared_mutex> lock(intrl_lock_);

			auto end = slots->slots_occupied().end();
			for(auto itr = slots->slots_occupied().begin(); itr != end; ++itr)
			{
			auto map_itr = slotted_entities_.lower_bound(*itr);
			if(map_itr != slotted_entities_.end() && !(slotted_entities_.key_comp()(*itr, map_itr->first)))
			{
				remove_intrl_(map_itr->second->QueryInterface<anh::api::SlotArrangementComponent>("SlotArrangement"));
				slotted_entities_.insert(std::make_pair<std::string, std::shared_ptr<Entity>>(*itr, what));
				permissions_->inserted(what);
			}
			else
			{
				slotted_entities_.insert(map_itr, std::map<std::string, std::shared_ptr<Entity>>::value_type(*itr, what));
				permissions_->inserted(what);
			}
				contained_objects_.insert(what);
			}
		}

		std::for_each(aware_entities_.begin(), aware_entities_.end(), [&] (std::shared_ptr<Entity> e) {
			//CALL TO KRONOS' CREATE GOES HERE
		});

		return true;
	}
	return false;
}

bool slotted_container_component::remove(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_removal)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_remove(who, what) || force_removal)
	{
		bool should_update = false;
		{ //Scope to allow the unique lock to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			remove_intrl_(what->QueryInterface<SlotArrangementComponent>("SlotArrangement"));
		}

		if(should_update)
		{
			std::for_each(aware_entities_.begin(), aware_entities_.end(), [&] (std::shared_ptr<Entity> e) {
				
				//Send Destroy for all Sub Objects
				what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
					//CALL TO KRONOS' DESTROY GOES HERE
				}, 0, false);
				
				//CALL TO KRONOS' DESTROY GOES HERE
			});
		}

		return true;
	}
	return false;
}

bool slotted_container_component::transfer_to(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion, bool force_removal)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(recv_container->permissions()->can_insert(who, what) || force_insertion)
	{
		if(permissions_->can_remove(who, what) || force_removal)
		{
			std::set<std::shared_ptr<Entity>> all_awares;

			std::set<std::shared_ptr<Entity>> recv_awares = recv_container->aware_entities(what);
			all_awares.insert(recv_awares.begin(), recv_awares.end());
			all_awares.insert(aware_entities_.begin(), aware_entities_.end());
			
			std::for_each(all_awares.begin(), all_awares.end(), [&] (std::shared_ptr<Entity> e) {
				bool in_recv = recv_awares.find(what) != recv_awares.end();
				bool in_send = aware_entities_.find(what) != aware_entities_.end();

				if(in_recv && in_send)
				{
					//CALL TO KRONOS' UPDATE CONTAINMENT GOES HERE
				}
				else if(!in_recv && in_send)
				{
					//Destroy all children for the e
					what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
						//CALL TO KRONOS' DESTROY GOES HERE
					}, 0, false);

					//CALL TO KRONOS' DESTROY GOES HERE
				}
				else
				{
					//CALL TO KRONOS' CREATE GOES HERE

					//Create all children for the e
					what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
						//CALL TO KRONOS' CREATE GOES HERE
					}, 0, true);
				}
			});

			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

			recv_container->intrl_insert_(what);
			remove_intrl_(what->QueryInterface<SlotArrangementComponent>("SlotArrangement"));
			permissions_->removed(what);

			return true;
		}
	}
	return false;
}