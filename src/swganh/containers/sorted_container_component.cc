#include "sorted_container_component.h"
#include <anh/component/entity.h>

#include <boost/thread/locks.hpp>

using namespace swganh::containers;
using namespace swganh::containers::permissions;
using namespace anh::component;

bool sorted_container_component::has_entity(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what)
{
	if(entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_view(who))
	{
		boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
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
	if(entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_view(who))
	{
		boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
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
	return entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->size() == 0;
}

bool sorted_container_component::full()
{
	auto permissions = entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions");
	return permissions->size() >= permissions->capacity();
}

size_t sorted_container_component::size()
{
	return entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->size();
}

size_t sorted_container_component::capacity()
{
	return entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->capacity();
}

bool sorted_container_component::capacity(size_t new_capacity)
{
	return entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->capacity(new_capacity);
}

bool sorted_container_component::insert(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, bool force_insertion)
{
	what->parent_intrl_(entity());
	auto permissions = entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions");
	if(force_insertion || permissions->can_insert(who, what))
	{
		boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
		{  //Scope to allow the unique lock to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			contained_objects_.insert(what);
			permissions->inserted(what);
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
	auto permissions = entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions");
	if( force_removal || permissions->can_remove(who, what))
	{
		bool should_update = false;
		boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
		{ //Scope to allow the unique lock to fall out of scope
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			if(contained_objects_.erase(what))
			{
				permissions->removed(what);
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
	if(force_removal || entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_remove(who, what))
	{
		if(force_insertion || recv_container->entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_insert(who, what))
		{
			boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
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
			entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->removed(what);

			return true;
		}
	}
	return false;
}

void sorted_container_component::make_aware(std::shared_ptr<anh::component::Entity> what)
{
	if(entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_view(what))
	{
		boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
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
	if(entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_view(what))
	{
		boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
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