#include "slotted_container_component.h"
#include <swganh/slots/slot_arrangement_component.h>
#include <anh/component/entity.h>

#include <boost/thread/locks.hpp>

using namespace swganh::containers;
using namespace swganh::slots;
using namespace anh::component;

bool slotted_container_component::intrl_insert_(std::shared_ptr<Entity> what, std::shared_ptr<ContainerComponentInterface> old_container)
{
	what->parent_intrl_(entity());

	bool ret_val = false;
	std::set<std::shared_ptr<Entity>> removed_objects;
	{
		boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);

		bool found_occupied = false;
		size_t occupied_arrangement = 0;

		//Okay, so this process is pretty simple, but must be done correctly.
		//First we want to make it possible to use any arrangement the object has in priority order.
		auto slot_arrangements = what->QueryInterface<SlotArrangementComponent>("SlotArrangement");
		for(size_t i = 0; i < slot_arrangements->arrangements(); ++i)
		{
			auto& arrangement = slot_arrangements->arrangement(i);
		
			bool is_occupied = false;
			bool is_bad = false;

			std::find_if(arrangement.begin(), arrangement.end(), [&, this] (const anh::HashString& s) -> bool {
				auto itr = slotted_entities_.find(s);
				if(itr != slotted_entities_.end())
				{
					if(!occupied_arrangement)
					{
						is_occupied = itr->second.entity != nullptr;
					}
					return false;
				}
				else
				{
					is_bad = true;
					return true;
				}
			});

			if(!is_bad) 
			{
				if(!is_occupied)
				{
					//Insert goes here
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
					
					std::for_each(arrangement.begin(), arrangement.end(), [&,this] (const anh::HashString& s) {
						auto itr = slotted_entities_.find(s);
						if(itr != slotted_entities_.end())
						{
							itr->second.entity = what;
							itr->second.arrangement_used = i;
						}
					});

					permissions_->inserted(what);
					return true;
				}
				else
				{
					found_occupied = true;
					occupied_arrangement = i;
				}
			}
		}
		
		if(found_occupied)
		{
			ret_val = true;
			boost::upgrade_to_unique_lock<boost::shared_mutex> unique(lock);
			std::find_if(slot_arrangements->arrangement(occupied_arrangement).begin(), slot_arrangements->arrangement(occupied_arrangement).end(), [&, this] (const anh::HashString& s) -> bool {
				auto itr = slotted_entities_.find(s);
				if(itr != slotted_entities_.end())
				{
					if(itr->second.entity != nullptr)
					{
						removed_objects.insert(itr->second.entity);
					}

					itr->second.entity = what;
					itr->second.arrangement_used = occupied_arrangement;
					return false;
				}
				else
				{
					ret_val = false;
					return true;
				}
			});
			
			if(ret_val)
			{
				permissions_->inserted(what);
			}
		}
	}

	if(ret_val)
	{
		//Below this point we have NO LOCKS ON ANYTHING. The functions we call will handle their own locking!!!
		std::for_each(removed_objects.begin(), removed_objects.end(), [&, this] (std::shared_ptr<Entity> e) 
		{
			//Insert the removed entity(ies) into the container the object we just equipped came from.
			//Otherwise we'll have to just let it disappear naturally.
			if(old_container != nullptr)
			{
				old_container->intrl_insert_(e, shared_from_this());
			}

			//Notify our permissions object
			permissions_->removed(e);
		});
	}
	return ret_val;
}

bool slotted_container_component::remove_intrl_(const std::set<anh::HashString>& slots)
{
	bool removed_something = false;

	auto end = slots.end();
	for(auto itr = slots.begin(); itr != end; ++itr)
	{
		auto dead_itr = slotted_entities_.find(*itr);
		if(dead_itr != slotted_entities_.end())
		{
			permissions_->removed(dead_itr->second.entity);
			contained_objects_.erase(dead_itr->second.entity);
			slotted_entities_.erase(dead_itr);
			removed_something = true;
		}
	}

	return removed_something;
}

std::shared_ptr<Entity> slotted_container_component::entity_in_slot(std::shared_ptr<Entity> who, std::string slot_name)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(who))
	{
		auto itr = slotted_entities_.find(slot_name);
		if(itr != slotted_entities_.end())
		{
			return itr->second.entity;
		}
	}
	return nullptr;
}

bool slotted_container_component::insert(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_insertion)
{
	what->parent_intrl_(entity());

	bool ret_val = false;

	intrl_lock_.lock_shared();
	if(force_insertion || permissions_->can_insert(who, what))
	{
		intrl_lock_.unlock_shared();
		ret_val = intrl_insert_(what, nullptr);
		if(ret_val)
		{
			boost::shared_lock<boost::shared_mutex> lock;

			std::for_each(aware_entities_.begin(), aware_entities_.end(), [] (std::shared_ptr<Entity> e) {
				//CALL TO KRONOS CREATE
			});;
		}
	}

	return ret_val;
}

bool slotted_container_component::insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, unsigned int id, bool force_insertion)
{
	bool ret_val = false;
	auto component = what->QueryInterface<SlotArrangementComponent>("SlotArrangement");
	std::set<std::shared_ptr<Entity>> removed_objects;
	if(id < component->arrangements())
	{
		const std::set<anh::HashString>& arrangement = component->arrangement(id);
		boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
		if(force_insertion || permissions_->can_insert(who, what, arrangement))
		{
			ret_val = true;
			boost::upgrade_to_unique_lock<boost::shared_mutex> unique(lock);
			std::find_if(arrangement.begin(), arrangement.end(), [&, this] (const anh::HashString& s) -> bool {
				auto itr = slotted_entities_.find(s);
				if(itr != slotted_entities_.end())
				{
					if(itr->second.entity != nullptr)
					{
						removed_objects.insert(itr->second.entity);
					}

					itr->second.entity = what;
					itr->second.arrangement_used = id;
					return false;
				}
				else
				{
					ret_val = false;
					return true;
				}
			});

			if(ret_val)
				permissions_->inserted(what);
		}
	}

	if(ret_val)
	{
		auto old_container = what->QueryInterface<ContainerComponentInterface>("Container");

		//Below this point we have NO LOCKS ON ANYTHING. The functions we call will handle their own locking!!!
		std::for_each(removed_objects.begin(), removed_objects.end(), [&, this] (std::shared_ptr<Entity> e) 
		{
			//Insert the removed entity(ies) into the container the object we just equipped came from.
			if(old_container != nullptr && old_container != ContainerComponentInterface::NullComponent)
			{
				this->transfer_to(e, e, old_container, true, true);
			}
			else
			{
				this->remove(e, e, true);
			}
		});
	}

	return ret_val;
}

bool slotted_container_component::remove(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_removal)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(force_removal || permissions_->can_remove(who, what))
	{
		bool should_update = false;
		{ //Scope to allow the unique lock to fall out of scope
			
			//Unfortunately we do not know which arrangement was used for this entity. So our only option is to iterate over the
			//slots we have until we find this entity and then get the arrangement id used.
			//It is at that point where we will unique lock the container and remove it
			
			bool found_arrangement = false;
			size_t arrangement_id = 0;

			auto end =  slotted_entities_.end();
			for(auto itr = slotted_entities_.begin(); itr != end; ++itr)
			{
				if(itr->second.entity == what)
				{
					found_arrangement = true;
					arrangement_id = itr->second.arrangement_used;
					break;
				}
			}
			
			if(found_arrangement)
			{
				boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
				should_update = remove_intrl_(what->QueryInterface<SlotArrangementComponent>("SlotArrangement")->arrangement(arrangement_id));
				permissions_->removed(what);
				should_update = true;
			}
		}

		if(should_update)
		{
			std::for_each(aware_entities_.begin(), aware_entities_.end(), [&] (std::shared_ptr<Entity> e) {
				
				//Send Destroy for all Sub Objects
				what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, false, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
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
	if(recv_container != nullptr && force_insertion || recv_container->permissions_can_insert(who, what))
	{
		if(force_removal || permissions_->can_remove(who, what))
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
					what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, false, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
						//CALL TO KRONOS' DESTROY GOES HERE
					}, 0, false);

					//CALL TO KRONOS' DESTROY GOES HERE
				}
				else
				{
					//CALL TO KRONOS' CREATE GOES HERE

					//Create all children for the e
					what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, false, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
						//CALL TO KRONOS' CREATE GOES HERE
					}, 0, true);
				}
			});

			bool found_arrangement = false;
			size_t arrangement_id = 0;

			auto end =  slotted_entities_.end();
			for(auto itr = slotted_entities_.begin(); itr != end; ++itr)
			{
				if(itr->second.entity == what)
				{
					found_arrangement = true;
					arrangement_id = itr->second.arrangement_used;
					break;
				}
			}

			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

			recv_container->intrl_insert_(what, shared_from_this());
			
			if(found_arrangement)
			{
				remove_intrl_(what->QueryInterface<SlotArrangementComponent>("SlotArrangement")->arrangement(arrangement_id));
				permissions_->removed(what);
			}

			return true;
		}
	}
	return false;
}

bool slotted_container_component::permissions_can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, const std::set<anh::HashString>& arrangement_to_use)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	return permissions_->can_insert(who, what, arrangement_to_use);
}