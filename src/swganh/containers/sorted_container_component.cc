#include "sorted_container_component.h"
#include <anh/component/entity.h>

#include <boost/thread/locks.hpp>

using namespace swganh::containers;
using namespace anh::component;

bool sorted_container_component::has_entity(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(who))
	{
		auto itr = contained_objects_.find(what);
		if(itr != contained_objects_.end())
		{
			return true;
		}
	}
	return false;
}

std::set<std::shared_ptr<anh::component::Entity>> sorted_container_component::aware_entities(std::shared_ptr<anh::component::Entity> caller_hint)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	return aware_entities_;
}

bool sorted_container_component::contained_objects(std::shared_ptr<anh::component::Entity> who, bool causes_populate, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth, bool top_down)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(who))
	{
		auto end = contained_objects_.end();
		if(top_down)
		{
			for(auto itr = contained_objects_.begin(); itr != end; ++itr)
			{
				funct(who, *itr);
				if(max_depth != 1)
				{
					(*itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, causes_populate, funct, (max_depth != 0) ? max_depth-1 : 0);
				}
			}
		}
		else
		{
			for(auto itr = contained_objects_.begin(); itr != end; ++itr)
			{
				if(max_depth != 1)
				{
					(*itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, causes_populate, funct, (max_depth != 0) ? max_depth-1 : 0);
				}
				funct(who, *itr);
			}
		}
		return true;
	}
	return false;
}



bool sorted_container_component::empty()
{
	boost::shared_lock<boost::shared_mutex>(intrl_lock_);
	return permissions_->size() == 0;
}

bool sorted_container_component::full()
{
	boost::shared_lock<boost::shared_mutex>(intrl_lock_);
	return permissions_->size() >= permissions_->capacity();
}

size_t sorted_container_component::size()
{
	boost::shared_lock<boost::shared_mutex>(intrl_lock_);
	return permissions_->size();
}

size_t sorted_container_component::capacity()
{
	boost::shared_lock<boost::shared_mutex>(intrl_lock_);
	return permissions_->capacity();
}

bool sorted_container_component::capacity(size_t new_capacity)
{
	boost::unique_lock<boost::shared_mutex>(intrl_lock_);
	return permissions_->capacity(new_capacity);
}

bool sorted_container_component::insert(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_insertion)
{
	what->parent_intrl_(entity());

	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(force_insertion || permissions_->can_insert(who, what))
	{
		{  //Scope to allow the unique lock to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			contained_objects_.insert(what);
			permissions_->inserted(what);
		}

		std::for_each(aware_entities_.begin(), aware_entities_.end(), [&] (std::shared_ptr<Entity> e) {
			//CALL TO KRONOS' CREATE GOES HERE
		});

		return true;
	}
	return false;
}

bool sorted_container_component::remove(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_removal)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if( force_removal || permissions_->can_remove(who, what))
	{
		bool should_update = false;
		{ //Scope to allow the unique lock to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			if(contained_objects_.erase(what))
			{
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

bool sorted_container_component::transfer_to(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion, bool force_removal)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(force_removal || permissions_->can_remove(who, what))
	{
		if(force_insertion || recv_container->permissions_can_insert(who, what))
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

					what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, true, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
						//CALL TO KRONOS' CREATE GOES HERE
					}, 0, true);
				}
			});

			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

			recv_container->intrl_insert_(what, shared_from_this());
			contained_objects_.erase(what);
			permissions_->removed(what);

			return true;
		}
	}
	return false;
}

void sorted_container_component::make_aware(std::shared_ptr<anh::component::Entity> what)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(what))
	{
		{ //Scope to allow uniqueness to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			aware_entities_.insert(what);
		}

		auto end = contained_objects_.end();
		for(auto itr = contained_objects_.begin(); itr != end; ++itr)
		{
			//Send Create in Duplex

			//Make aware any children
			(*itr)->QueryInterface<ContainerComponentInterface>("Container")->make_aware(what);
		}
	}
}

void sorted_container_component::state_update(std::shared_ptr<anh::component::Entity> what, const glm::vec3& oldpos, const glm::vec3& newpos, const glm::quat& rotation)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	std::for_each(aware_entities_.begin(), aware_entities_.end(), [&] (std::shared_ptr<Entity> e) {
		//Send Movement Update
	});
}

void sorted_container_component::make_unaware(std::shared_ptr<anh::component::Entity> what)
{
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(what))
	{
		auto end = contained_objects_.end();
		for(auto itr = contained_objects_.begin(); itr != end; ++itr)
		{
			//Make unaware any children
			(*itr)->QueryInterface<ContainerComponentInterface>("Container")->make_unaware(what);

			//Send Destroy in Duplex
		}

		boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
		aware_entities_.erase(what);
	}
}

bool sorted_container_component::intrl_insert_(std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> old_container)
{
	what->parent_intrl_(entity());

	boost::unique_lock<boost::shared_mutex> lock(intrl_lock_);
	contained_objects_.insert(what);
	return true;
}

bool sorted_container_component::permissions_grant_view(std::shared_ptr<Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_grant_view(who, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 1
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_revoke_view(std::shared_ptr<Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_revoke_view(who, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 0
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_grant_insert(std::shared_ptr<Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_grant_insert(who, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 1
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_revoke_insert(std::shared_ptr<Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_revoke_insert(who, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 0
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_grant_removal(std::shared_ptr<Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_grant_removal(who, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 1
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_revoke_removal(std::shared_ptr<Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_revoke_removal(who, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 0
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_grant_view(std::string argument)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_grant_view(argument, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 1
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_revoke_view(std::string argument)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_revoke_view(argument, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 0
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_grant_insert(std::string argument)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_grant_insert(argument, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 1
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_revoke_insert(std::string argument)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_revoke_insert(argument, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 0
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_grant_removal(std::string argument)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_grant_removal(argument, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 1
		
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_revoke_removal(std::string argument)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->accepts_changes())
	{
		std::set<std::shared_ptr<Entity>> delta_set = permissions_->permissions_revoke_removal(argument, aware_entities_);
		std::for_each(delta_set.begin(), delta_set.end(), [&] (std::shared_ptr<Entity> e) {
			//Send UpdateCellPermissionMessage with flag = 0
		});
		return true;
	}
	return false;
}

bool sorted_container_component::permissions_can_view(std::shared_ptr<anh::component::Entity> who)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	return permissions_->can_view(who);
}

bool sorted_container_component::permissions_can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	return permissions_->can_insert(who, what);
}

bool sorted_container_component::permissions_can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	return permissions_->can_remove(who, what);
}