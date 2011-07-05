#include <container_system/containers/sorted_container_component.h>
#include <anh/component/entity.h>

#include <boost/thread/locks.hpp>

using namespace spatial_index::container;
using namespace anh::api;
using namespace anh::api::components;
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

bool sorted_container_component::contained_objects(std::shared_ptr<anh::component::Entity> who, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth, bool top_down)
{
	boost::shared_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_view(who))
	{
		auto end = contained_objects_.end();
		for(auto itr = contained_objects_.begin(); itr != end; ++itr)
		{
			if(top_down)
			{
				funct(who, *itr);
				if(max_depth != 1)
				{
					(*itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, funct, (max_depth != 0) ? max_depth-1 : 0);
				}
			}
			else
			{
				if(max_depth != 1)
				{
					(*itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, funct, (max_depth != 0) ? max_depth-1 : 0);
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
	boost::upgrade_lock<boost::shared_mutex> lock(intrl_lock_);
	if(permissions_->can_insert(who, what) || force_insertion)
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
	if(permissions_->can_remove(who, what) || force_removal)
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

bool sorted_container_component::transfer_to(std::shared_ptr<Entity> who, std::shared_ptr<Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion, bool force_removal)
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

					what->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(e, [] (std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
						//CALL TO KRONOS' CREATE GOES HERE
					}, 0, true);
				}
			});

			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

			recv_container->intrl_insert_(what);
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

void sorted_container_component::state_update(std::shared_ptr<anh::component::Entity> what, glm::vec3& old)
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