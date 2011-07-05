#include <container_system/containers/universe_container_component.h>
#include <mod_anh_transform/transform_component.h>

#include <math.h>

using namespace container_system;
using namespace anh::api;
using namespace anh::api::components;

int universe_container_component::universe_bucket::next_id = 0;

universe_container_component::universe_container_component(float map_size, float bucket_size, float viewing_range, bool _3_dimensional_)
	: ContainerComponentInterface("Universe")
	, half_map_width_(map_size/2)
	, bucket_width_(bucket_size)
	, viewing_range_(viewing_range)
	, buckets_per_row_(ceil(map_size / bucket_size))
	, is_3D_(_3_dimensional_)
{
	if(_3_dimensional_)
	{
		for(unsigned int i = 0; i < buckets_per_row_; ++i)
			for(unsigned int j = 0; j < buckets_per_row_; ++j)
				for(unsigned int k =0; k < buckets_per_row_; ++k)
					buckets_.push_back(std::make_shared<universe_bucket>());

		//Generate the relevant buckets
		int depth = (int)ceil(viewing_range_ / bucket_width_);
		std::for_each(buckets_.begin(), buckets_.end(), [&, this] (std::shared_ptr<universe_bucket> bucket) {
			generate_relevant_buckets_3D_(depth, bucket);
		});
	}
	else
	{
		for(unsigned int i = 0; i < buckets_per_row_; ++i)
			for(unsigned int j = 0; j < buckets_per_row_; ++j)
				buckets_.push_back(std::make_shared<universe_bucket>());

		//Generate the relevant buckets
		int depth = (int)ceil(viewing_range_ / bucket_width_);
		std::for_each(buckets_.begin(), buckets_.end(), [&, this] (std::shared_ptr<universe_bucket> bucket) {
			generate_relevant_buckets_2D_(depth, bucket);
		});
	}
}

bool universe_container_component::insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion)
{
	//Get the Transform Component
	auto transform = what->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");

	//Find the associated bucket
	auto assoc_bucket = buckets_[relevant_bucket_(transform->position())];
	
	auto end = assoc_bucket->relevant_buckets_.end();
	auto itr = assoc_bucket->relevant_buckets_.begin();

	//For Every Bucket in Associated Bucket Relevant Bucket list
	for(; itr != end; ++itr)
	{
		//Lock Bucket
		if(*itr == assoc_bucket)
			(*itr)->mutex_.lock();
		else
			(*itr)->mutex_.lock_shared();
	} //End For

	//Insert into the associated bucket
	assoc_bucket->contained_objects_.insert(what);
	
	itr = assoc_bucket->relevant_buckets_.begin();
	//For Every Bucket in Associated Bucket Relevant Bucket list
	for(; itr != end; ++itr)
	{
		//For Every Object in the Bucket
		std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
			//Send Baseline
			//call make_aware on the object
			e->QueryInterface<ContainerComponentInterface>("Container")->make_aware(who);
		}); //End For
	} //End For

	//For Every Bucket in Associated Bucket Relevant Bucket list
	for(; itr != end; ++itr)
	{
		//UnLock Bucket
		if(*itr == assoc_bucket)
			(*itr)->mutex_.unlock();
		else
			(*itr)->mutex_.unlock_shared();
	} //End For

	return true;
}

bool universe_container_component::intrl_insert_(std::shared_ptr<anh::component::Entity> what)
{
	//Get the Transform Component
	auto transform = what->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");

	//Find the associated bucket
	auto assoc_bucket = buckets_[relevant_bucket_(transform->position())];
	
	assoc_bucket->mutex_.lock();
	assoc_bucket->contained_objects_.insert(what);
	assoc_bucket->mutex_.unlock();

	return true;
}

bool universe_container_component::remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal)
{
	//Get the Transform Component
	auto transform = what->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");

	//Find the associated bucket
	auto assoc_bucket = buckets_[relevant_bucket_(transform->position())];

	//For Every Bucket in Associated Bucket Relevant Bucket list
	auto itr = assoc_bucket->relevant_buckets_.begin();
	auto end = assoc_bucket->relevant_buckets_.end();
	for(; itr != end; ++itr)
	{
		//Lock Bucket
		if(*itr == assoc_bucket)
			(*itr)->mutex_.lock();
		else
			(*itr)->mutex_.lock_shared();
	} //End For

	//Remove from the associated bucket
	assoc_bucket->contained_objects_.erase(what);
	
	itr = assoc_bucket->relevant_buckets_.begin();
	//For Every Bucket in Associated Bucket Relevant Bucket list
	for(; itr != end; ++itr)
	{
		//For Every Object in the Bucket
		std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
			//Send Destroy in Duplex
			//call make_aware on the object
			e->QueryInterface<ContainerComponentInterface>("Container")->make_unaware(who);
		}); //End For
	} //End For

	//For Every Bucket in Associated Bucket Relevant Bucket list
	for(; itr != end; ++itr)
	{
		//UnLock Bucket
		if(*itr == assoc_bucket)
			(*itr)->mutex_.unlock();
		else
			(*itr)->mutex_.unlock_shared();
	} //End For

	return true;
}

bool universe_container_component::transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion, bool force_removal)
{
	//If permissions will let this happen
	if(recv_container->permissions()->can_insert(who, what) || force_insertion)
	{
		//Get the Transform Component
		auto transform_what = what->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");
		auto transform_recv = recv_container->entity()->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");

		//Find the associated bucket with the old parent
		auto old_parent = buckets_[relevant_bucket_(transform_what->position())];

		//Find the associated bucket with the new parent
		auto new_parent = buckets_[relevant_bucket_(transform_recv->position())];

		//If the buckets are the same.
		if(old_parent == new_parent)
		{
			auto end = old_parent->relevant_buckets_.end();
			auto itr = old_parent->relevant_buckets_.begin();
			//For Every Bucket in Associated Bucket Relevant Bucket List
			for(; itr != end; ++itr)
			{
				//Lock Bucket
				(*itr)->mutex_.lock_shared();
			} //End For

			//Switch Parents

			//intrl_insert_ is an internal insertion which only fails if the container we're transfering to
			//is not actually a container, and cannot contain anything.
			if(!recv_container->intrl_insert_(what))
			{
				itr = old_parent->relevant_buckets_.begin();
				for(; itr != end; ++itr)
					(*itr)->mutex_.unlock_shared();
				return false;
			}

			old_parent->contained_objects_.erase(what);

			//For Every Bucket in Associated Bucket Relevant Bucket list
			itr = old_parent->relevant_buckets_.begin();
			for(; itr != end; ++itr)
			{
				//For Every Object in the Bucket
				std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
					//Send Containment Update
					//Call Contained_Objects() for Containment Update
				}); //End For

				//Unlock Bucket to save some time
				(*itr)->mutex_.unlock_shared();
			} //End For
		} 
		else
		{
			//Create a Set for every bucket in both associated bucket relevant lists
			std::set<std::shared_ptr<universe_bucket>> all_buckets;
			all_buckets.insert(old_parent->relevant_buckets_.begin(), old_parent->relevant_buckets_.end());
			all_buckets.insert(new_parent->relevant_buckets_.begin(), new_parent->relevant_buckets_.end());

			//For Every Bucket in The set
			
			auto end = all_buckets.end();
			auto itr = all_buckets.begin();
			for(; itr != end; ++itr)
			{
				//Lock Bucket
				if(*itr == old_parent || *itr == new_parent)
				{
					(*itr)->mutex_.lock();
				}
				else
				{
					(*itr)->mutex_.lock_shared();
				}
			} //End For

			//Switch Parents
			recv_container->intrl_insert_(what);
			old_parent->contained_objects_.erase(what);

			//For Every Bucket in the set
			itr = all_buckets.begin();
			for(; itr != end; ++itr)
			{
				//If Bucket is in BOTH Old and new associated bucket lists
				bool in_old = old_parent->relevant_buckets_.find(*itr) != old_parent->relevant_buckets_.end();
				bool in_new = new_parent->relevant_buckets_.find(*itr) != new_parent->relevant_buckets_.end();
				if(in_old && in_new)
				{
					//For Every Object in Bucket
					std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
						//Send Containement Update
						//Call Contained_Objects() for Containement Update
					}); //End For
				}
				else if(in_old && !in_new)
				{
					//For Every Object in Bucket
					std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
						//Call make_unaware for object
						e->QueryInterface<ContainerComponentInterface>("Container")->make_unaware(what);
						//Send Destroy in Duplex
					}); //End For
				}
				else
				{
					//For Every Object in Bucket
					std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
						//Send Create in Duplex
						//Call make_aware for object
						e->QueryInterface<ContainerComponentInterface>("Container")->make_aware(what);
					}); //End For
				}
			} //End For

			itr = all_buckets.begin();
			for(; itr != end; ++itr)
			{
				//Lock Bucket
				if(*itr == old_parent || *itr == new_parent)
				{
					(*itr)->mutex_.unlock();
				}
				else
				{
					(*itr)->mutex_.unlock_shared();
				}
			} //End For
		}

		return true;
	}

	return false;
}

std::set<std::shared_ptr<anh::component::Entity>> universe_container_component::aware_entities(std::shared_ptr<anh::component::Entity> caller_hint)
{
	std::set<std::shared_ptr<anh::component::Entity>> result_set;

	//Use the caller_hint to get the caller's transform
	auto transform = caller_hint->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");
	
	//With the transform, find the bucket the caller is in.
	auto parent = buckets_[relevant_bucket_(transform->position())];

	//Lock all the buckets (shared)
	auto end = parent->relevant_buckets_.end();
	auto itr = parent->relevant_buckets_.begin();
	for(; itr != end; ++itr)
	{
		(*itr)->mutex_.lock_shared();
	}

	//Call the function provided for all children
	itr = parent->relevant_buckets_.begin();
	for(; itr != end; ++itr)
	{
		auto sub_itr = (*itr)->contained_objects_.begin();
		auto sub_end = (*itr)->contained_objects_.end();
		for(; sub_itr != sub_end; ++sub_itr)
		{
			result_set.insert(*sub_itr);
			(*sub_itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(entity(), [&] (std::shared_ptr<Entity> t, std::shared_ptr<Entity> e) {
				result_set.insert(e);
			});
		}

		//Unlock the bucket
		(*itr)->mutex_.unlock_shared();
	}

	return result_set;
}

void universe_container_component::state_update(std::shared_ptr<anh::component::Entity> what, glm::vec3& old)
{
	//Get the Transform Component
	auto transform = what->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");

	//Find the associated bucket with the old parent
	auto old_parent = buckets_[relevant_bucket_(old)];

	//Find the associated bucket with the new parent
	auto new_parent = buckets_[relevant_bucket_(transform->position())];

	//If the buckets are the same.
	if(old_parent == new_parent)
	{
		auto end = old_parent->relevant_buckets_.end();
		auto itr = old_parent->relevant_buckets_.begin();
		//For Every Bucket in Associated Bucket Relevant Bucket List
		for(; itr != end; ++itr)
		{
			//Lock Bucket
			(*itr)->mutex_.lock_shared();
		} //End For

		//For Every Bucket in Associated Bucket Relevant Bucket list
		itr = old_parent->relevant_buckets_.begin();
		for(; itr != end; ++itr)
		{
			//For Every Object in the Bucket
			std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
				//Send Movement Update
				//Call Contained_Objects() for Containment Update
			}); //End For

			//Unlock Bucket to save some time
			(*itr)->mutex_.unlock_shared();
		} //End For
	} 
	else
	{
		//Create a Set for every bucket in both associated bucket relevant lists
		std::set<std::shared_ptr<universe_bucket>> all_buckets;
		all_buckets.insert(old_parent->relevant_buckets_.begin(), old_parent->relevant_buckets_.end());
		all_buckets.insert(new_parent->relevant_buckets_.begin(), new_parent->relevant_buckets_.end());

		//For Every Bucket in The set
			
		auto end = all_buckets.end();
		auto itr = all_buckets.begin();
		for(; itr != end; ++itr)
		{
			//Lock Bucket
			if(*itr == old_parent || *itr == new_parent)
			{
				(*itr)->mutex_.lock();
			}
			else
			{
				(*itr)->mutex_.lock_shared();
			}
		} //End For

		//For Every Bucket in the set
		itr = all_buckets.begin();
		for(; itr != end; ++itr)
		{
			//If Bucket is in BOTH Old and new associated bucket lists
			bool in_old = old_parent->relevant_buckets_.find(*itr) != old_parent->relevant_buckets_.end();
			bool in_new = new_parent->relevant_buckets_.find(*itr) != new_parent->relevant_buckets_.end();
			if(in_old && in_new)
			{
				//For Every Object in Bucket
				std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
					//Send Movement Update
					//Call Contained_Objects() for Containement Update
				}); //End For
			}
			else if(in_old && !in_new)
			{
				//For Every Object in Bucket
				std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
					//Call make_unaware for object
					e->QueryInterface<ContainerComponentInterface>("Container")->make_unaware(what);
					//Send Destroy in Duplex
				}); //End For
			}
			else
			{
				//For Every Object in Bucket
				std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
					//Send Create in Duplex
					//Call make_aware for object
					e->QueryInterface<ContainerComponentInterface>("Container")->make_aware(what);
				}); //End For
			}
		} //End For

		itr = all_buckets.begin();
		for(; itr != end; ++itr)
		{
			//Lock Bucket
			if(*itr == old_parent || *itr == new_parent)
			{
				(*itr)->mutex_.unlock();
			}
			else
			{
				(*itr)->mutex_.unlock_shared();
			}
		} //End For
	}
}

bool universe_container_component::contained_objects(std::shared_ptr<anh::component::Entity> who, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth)
{
	//Use the caller_hint to get the caller's transform
	auto transform = who->QueryInterface<anh::api::components::TransformComponentInterface>("Transform");
	
	//With the transform, find the bucket the caller is in.
	auto parent = buckets_[relevant_bucket_(transform->position())];

	//Lock all the buckets (shared)
	auto end = parent->relevant_buckets_.end();
	auto itr = parent->relevant_buckets_.begin();
	for(; itr != end; ++itr)
	{
		(*itr)->mutex_.lock_shared();
	}

	//Call the function provided for all children
	itr = parent->relevant_buckets_.begin();
	for(; itr != end; ++itr)
	{
		auto sub_itr = (*itr)->contained_objects_.begin();
		auto sub_end = (*itr)->contained_objects_.end();
		for(; sub_itr != sub_end; ++sub_itr)
		{
			funct(who, *sub_itr);

			if(max_depth != 1)
			{
				(*sub_itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, funct, (max_depth) ? max_depth-1 : 0);
			}
		}

		//Unlock the bucket
		(*itr)->mutex_.unlock_shared();
	}

	return true;
}

void universe_container_component::generate_relevant_buckets_2D_(int depth, std::shared_ptr<universe_bucket> bucket)
{	
	bucket->relevant_buckets_.clear();

	//Now we actually do it.
	int low_x = (bucket->id % (int)buckets_per_row_) - depth;
	int high_x = (bucket->id % (int)buckets_per_row_) + depth;

	int low_z = (bucket->id - low_x) / buckets_per_row_ - depth;
	int high_z = (bucket->id - low_x) / buckets_per_row_ + depth;

	if(low_x < 0)
		low_x = 0;
	if(high_x >= (int)buckets_per_row_)
		high_x = buckets_per_row_-1;
	if(low_z < 0)
		low_z = 0;
	if(high_z >= (int)buckets_per_row_)
		high_z = buckets_per_row_-1;

	for(int z = low_z; z <= high_z; ++z)
	{
		for(int x = low_x; x <= high_x; ++x)
		{
			bucket->relevant_buckets_.insert(buckets_[x + z*buckets_per_row_]);
		}
	}
}

void universe_container_component::generate_relevant_buckets_3D_(int depth, std::shared_ptr<universe_bucket> bucket)
{	
	
}

size_t universe_container_component::relevant_bucket_(const glm::vec3& pos)
{
	if(is_3D_)
		return relevant_bucket_3D_(pos);
	else
		return relevant_bucket_2D_(pos);
}

size_t universe_container_component::relevant_bucket_2D_(const glm::vec3& pos)
{
	size_t x = (pos.x + half_map_width_) / buckets_per_row_;
	size_t z = (pos.z + half_map_width_) / buckets_per_row_;

	return x +  z * buckets_per_row_;
}

size_t universe_container_component::relevant_bucket_3D_(const glm::vec3& pos)
{
	size_t x = (pos.x + half_map_width_) / buckets_per_row_;
	size_t y = (pos.y + half_map_width_) / buckets_per_row_;
	size_t z = (pos.z + half_map_width_) / buckets_per_row_;

	return x + y * buckets_per_row_ * buckets_per_row_ + z * buckets_per_row_;
}

std::shared_ptr<ContainerPermissionsInterface> universe_container_component::permissions()
{
	//Not relevant for the universe container!
	return nullptr;
}

bool universe_container_component::permissions(std::shared_ptr<ContainerPermissionsInterface> new_permissions)
{
	//Not relevant for the universe container!
	return false;
}

bool universe_container_component::empty()
{
	//Not relevant for the universe container!
	return false;
}

bool universe_container_component::full()
{
	//Not relevant for the universe container!
	return false;
}

size_t universe_container_component::size()
{
	//Not relevant for the universe container!
	return 0;
}

size_t universe_container_component::capacity()
{
	//Not relevant for the universe container!
	return 0;
}

bool universe_container_component::capacity(size_t new_capacity)
{
	//Not relevant for the universe container!
	return false;
}

//Notification Methods
void universe_container_component::make_aware(std::shared_ptr<anh::component::Entity> what)
{
	//Not relevant for the universe container!
}
	
void universe_container_component::make_unaware(std::shared_ptr<anh::component::Entity> what)
{
	//Not relevant for the universe container!
}

bool universe_container_component::has_entity(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what)
{
	//Not relevant for the universe container!
	return true;
}

std::shared_ptr<anh::component::Entity> universe_container_component::entity_in_slot(std::shared_ptr<anh::component::Entity> who,std::string slot_name)
{
	//Not relevant for the universe container!
	return nullptr;
}