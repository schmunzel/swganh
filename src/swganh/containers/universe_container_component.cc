#include "universe_container_component.h"

#include <boost/thread/locks.hpp>
#include <glog//logging.h>
#include <limits>
#include <math.h>

#include <swganh/transform/transform_component.h>
#include <swganh/component/connection_component_interface.h>
#include <anh/component/entity.h>

#include <swganh/baseline/baseline_event.h>


using namespace swganh::transform;
using namespace swganh::baseline;
using namespace swganh::containers;
using namespace swganh::component;
using namespace swganh::containers::permissions;
using namespace swganh::regions;
using namespace anh::component;

universe_container_component::universe_container_component() : ContainerComponentInterface("Universe") {}

universe_container_component::universe_container_component(float map_size, float bucket_size, float viewing_range, bool _3_dimensional_)
	: ContainerComponentInterface("Universe")
	, half_map_width_(map_size/2)
	, bucket_width_(bucket_size)
	, viewing_range_(viewing_range)
	, buckets_per_row_(ceil(map_size / bucket_size))
	, buckets_sq(buckets_per_row_*buckets_per_row_)
	, is_3D_(_3_dimensional_)
{
	construct_();
}
void universe_container_component::Init(boost::property_tree::ptree& pt)
{
	float map_size = pt.get<float>("map_size", 16384);
	bucket_width_ = pt.get<float>("bucket_size", 64);
	viewing_range_ = pt.get<float>("viewing_range", 128);
	is_3D_ = pt.get<bool>("is_3D", false);

	half_map_width_ = map_size/2;
	buckets_per_row_ = ceil(map_size / bucket_width_);
	buckets_sq = buckets_per_row_*buckets_per_row_;

    DLOG(WARNING) << "Started Loading Universe...";
	construct_();
    DLOG(WARNING) << "Finished Loading Universe...";
}

void universe_container_component::construct_()
{
	size_t id = 0;
	for(unsigned int i = 0; i < ((is_3D_) ? buckets_per_row_ : 1); ++i) 
	{
		for(unsigned int j = 0; j < buckets_per_row_; ++j) 
		{
			for(unsigned int k =0; k < buckets_per_row_; ++k) 
			{
				buckets_.push_back(std::make_shared<universe_bucket>(id++));
			}
		}
	}
	//Generate the relevant buckets
	int depth = (int)ceil(viewing_range_ / bucket_width_);
	std::for_each(buckets_.begin(), buckets_.end(), [&, this] (std::shared_ptr<universe_bucket> bucket) {
		generate_relevant_buckets_(depth, bucket);
	});
}

bool universe_container_component::insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion)
{
	what->parent_intrl_(entity());

	//Get the Transform Component
	auto transform = what->QueryInterface<swganh::transform::TransformComponentInterface>("Transform");

	//Prep the player regions before we even lock the buckets.
	auto player_regions = transform->regions();

	//Find the associated bucket
	auto position = transform->position();
	auto assoc_bucket = buckets_[relevant_bucket_(position)];
	
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
	
	//Do region updates.
	auto region_end = assoc_bucket->contained_regions_.end();
	for(auto region_itr = assoc_bucket->contained_regions_.begin(); region_itr != region_end; ++region_itr)
	{
		bool within = (*region_itr)->within_extent(position);
		bool has = player_regions.find(*region_itr) != player_regions.end();

		if(!within && has)
		{
			transform->remove_region(*region_itr);
			(*region_itr)->on_exit(what);
		}
		else if(within && !has)
		{
			transform->insert_region(*region_itr);
			(*region_itr)->on_enter(what);
		}
	}

	auto baseline_event = anh::event_dispatcher::make_shared_event("BaselineEvent", BaselineEvent(what, true));

	//For Every Bucket in Associated Bucket Relevant Bucket list
	itr = assoc_bucket->relevant_buckets_.begin();
	for(; itr != end; ++itr)
	{
		//For Every Object in the Bucket
		std::for_each((*itr)->contained_objects_.begin(), (*itr)->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
			baseline_event->receiving_entities_.push_back(e);

			//call make_aware on the object
			e->QueryInterface<ContainerComponentInterface>("Container")->make_aware(what);
			what->QueryInterface<ContainerComponentInterface>("Container")->make_aware(e);
		}); //End For
	} //End For

	//Send Event
	what->QueryInterface<ConnectionComponentInterface>("connection")->session()->server()->event_dispatcher()->triggerAsync(baseline_event);

	//Insert into the associated bucket
	assoc_bucket->contained_objects_.insert(what);

	//For Every Bucket in Associated Bucket Relevant Bucket list
	itr = assoc_bucket->relevant_buckets_.begin();
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

bool universe_container_component::intrl_insert_(std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> old_container)
{
	what->parent_intrl_(entity());

	//Get the Transform Component
	auto transform = what->QueryInterface<TransformComponentInterface>("Transform");

	auto player_regions = transform->regions();

	//Find the associated bucket
	auto position = transform->position();
	auto assoc_bucket = buckets_[relevant_bucket_(position)];
	
	assoc_bucket->mutex_.lock();
	assoc_bucket->contained_objects_.insert(what);

	//Do region updates.
	auto region_end = assoc_bucket->contained_regions_.end();
	for(auto region_itr = assoc_bucket->contained_regions_.begin(); region_itr != region_end; ++region_itr)
	{
		bool within = (*region_itr)->within_extent(position);
		bool has = player_regions.find(*region_itr) != player_regions.end();

		if(!within && has)
			transform->remove_region(*region_itr);
		else if(within && !has)
			transform->insert_region(*region_itr);
	}

	assoc_bucket->mutex_.unlock();

	return true;
}

bool universe_container_component::remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal)
{
	//Get the Transform Component
	auto transform = what->QueryInterface<TransformComponentInterface>("Transform");

	auto player_regions = transform->regions();

	//Find the associated bucket
	auto position = transform->position();
	auto cur_parent = buckets_[relevant_bucket_(position)];

	//For Every Bucket in Associated Bucket Relevant Bucket list
	auto itr = cur_parent->relevant_buckets_.begin();
	auto end = cur_parent->relevant_buckets_.end();
	for(; itr != end; ++itr)
	{
		//Lock Bucket
		if(*itr == cur_parent)
			(*itr)->mutex_.lock();
		else
			(*itr)->mutex_.lock_shared();
	} //End For

	//Remove from the associated bucket
	cur_parent->contained_objects_.erase(what);

	//Do region updates.
	auto region_end = cur_parent->contained_regions_.end();
	for(auto region_itr = cur_parent->contained_regions_.begin(); region_itr != region_end; ++region_itr)
	{
		bool within = (*region_itr)->within_extent(position);
		bool has = player_regions.find(*region_itr) != player_regions.end();

		if(!within && has)
		{
			transform->remove_region(*region_itr);
			(*region_itr)->on_exit(what);
		}
		else if(within && !has)
		{
			transform->insert_region(*region_itr);
			(*region_itr)->on_enter(what);
		}
	}

	//For Every Bucket in Associated Bucket Relevant Bucket list
	itr = cur_parent->relevant_buckets_.begin();
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
	itr = cur_parent->relevant_buckets_.begin();
	for(; itr != end; ++itr)
	{
		//UnLock Bucket
		if(*itr == cur_parent)
			(*itr)->mutex_.unlock();
		else
			(*itr)->mutex_.unlock_shared();
	} //End For

	return true;
}

bool universe_container_component::transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion, bool force_removal)
{
	//If permissions will let this happen
	if(force_insertion || recv_container->entity()->QueryInterface<ContainerPermissionsInterface>("ContainerPermissions")->can_insert(who, what))
	{
		//Get the Transform Component
		auto transform_what = what->QueryInterface<TransformComponentInterface>("Transform");
		auto transform_recv = recv_container->entity()->QueryInterface<TransformComponentInterface>("Transform");

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
			if(!recv_container->intrl_insert_(what, shared_from_this()))
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
			recv_container->intrl_insert_(what, shared_from_this());
			old_parent->contained_objects_.erase(what);

			auto baseline_event = anh::event_dispatcher::make_shared_event("BaselineEvent", BaselineEvent(what, true));

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
						baseline_event->receiving_entities_.push_back(e);

						//Call make_aware for object
						e->QueryInterface<ContainerComponentInterface>("Container")->make_aware(what);
					}); //End For
				}
			} //End For

			what->QueryInterface<ConnectionComponentInterface>("connection")->session()->server()->event_dispatcher()->triggerAsync(baseline_event);

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
	auto transform = caller_hint->QueryInterface<TransformComponentInterface>("Transform");
	
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
	for(itr = parent->relevant_buckets_.begin(); itr != end; ++itr)
	{
		auto sub_itr = (*itr)->contained_objects_.begin();
		auto sub_end = (*itr)->contained_objects_.end();
		for(; sub_itr != sub_end; ++sub_itr)
		{
			result_set.insert(*sub_itr);
			(*sub_itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(entity(), false, [&] (std::shared_ptr<Entity> t, std::shared_ptr<Entity> e) { 
				result_set.insert(e); 
			});
		}

		//Unlock the bucket
		(*itr)->mutex_.unlock_shared();
	}

	return result_set;
}

void universe_container_component::state_update(std::shared_ptr<anh::component::Entity> what, const glm::vec3& oldpos, const glm::vec3& newpos, const glm::quat& rotation)
{
	//Find the associated bucket with the old parent
	auto old_parent = buckets_[relevant_bucket_(oldpos)];
	auto new_parent = buckets_[relevant_bucket_(newpos)];

	if(old_parent == new_parent)
	{
		bool rotation_only = oldpos == newpos;

		auto end = old_parent->relevant_buckets_.end();
		auto itr = old_parent->relevant_buckets_.begin();
		
		auto player_regions = what->QueryInterface<TransformComponentInterface>("Transform")->regions();

		std::set<std::shared_ptr<swganh::regions::RegionInterface>> all_regions;

		if(!rotation_only)
		{
			all_regions.insert(player_regions.begin(), player_regions.end());
		}

		//For Every Bucket in Associated Bucket Relevant Bucket List
		for(; itr != end; ++itr)
		{
			(*itr)->mutex_.lock_shared();
		}

		if(!rotation_only)
		{
			//Do region updates.
			all_regions.insert(old_parent->contained_regions_.begin(), old_parent->contained_regions_.end());
			std::for_each(all_regions.begin(), all_regions.end(), [&] (std::shared_ptr<RegionInterface> r) {
				bool within = r->within_extent(newpos);
				bool has = player_regions.find(r) != player_regions.end();

				if(!within && has)
				{
					r->on_exit(what);
				}
				else if(within && !has)
				{
					r->on_enter(what);
				}

			});
		}

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
		}
	}
	else
	{

		//Create a Set for every bucket in both associated bucket relevant lists
		std::set<std::shared_ptr<universe_bucket>> all_buckets;
		all_buckets.insert(old_parent->relevant_buckets_.begin(), old_parent->relevant_buckets_.end());
		all_buckets.insert(new_parent->relevant_buckets_.begin(), new_parent->relevant_buckets_.end());

		auto player_regions = what->QueryInterface<TransformComponentInterface>("Transform")->regions();

		std::set<std::shared_ptr<RegionInterface>> all_regions;
		all_regions.insert(player_regions.begin(), player_regions.end());

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

		//Do region updates.
		all_regions.insert(new_parent->contained_regions_.begin(), new_parent->contained_regions_.end());
		std::for_each(all_regions.begin(), all_regions.end(), [&] (std::shared_ptr<RegionInterface> r) {

			bool within = r->within_extent(newpos);
			bool has = player_regions.find(r) != player_regions.end();

			if(!within && has)
			{
				r->on_exit(what);
			}
			else if(within && !has)
			{
				r->on_enter(what);
			}

		});

		auto baseline_event = anh::event_dispatcher::make_shared_event("BaselineEvent", BaselineEvent(what, true));

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
					baseline_event->receiving_entities_.push_back(e);

					//Call make_aware for object
					e->QueryInterface<ContainerComponentInterface>("Container")->make_aware(what);
				}); //End For
			}
		} //End For

		what->QueryInterface<ConnectionComponentInterface>("connection")->session()->server()->event_dispatcher()->triggerAsync(baseline_event);

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

bool universe_container_component::contained_objects(std::shared_ptr<anh::component::Entity> who, bool causes_populate, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth, bool top_down)
{
	//Use the caller_hint to get the caller's transform
	auto transform = who->QueryInterface<TransformComponentInterface>("Transform");
	
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
			if(top_down)
			{
				funct(who, *sub_itr);
				if(max_depth != 1)
				{
					(*sub_itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, causes_populate, funct, (max_depth) ? max_depth-1 : 0);
				}
			}
			else
			{
				if(max_depth != 1)
				{
					(*sub_itr)->QueryInterface<ContainerComponentInterface>("Container")->contained_objects(who, causes_populate, funct, (max_depth) ? max_depth-1 : 0);
				}
				funct(who, *sub_itr);
			}
		}

		//Unlock the bucket
		(*itr)->mutex_.unlock_shared();
	}

	return true;
}

void universe_container_component::insert(std::shared_ptr<RegionInterface> region)
{
	std::set<size_t> buckets = buckets_within_verts_(region->verts());
	std::for_each(buckets.begin(), buckets.end(), [&, this] (size_t id) {
		std::shared_ptr<universe_bucket> bucket = buckets_[id];

		boost::upgrade_lock<boost::shared_mutex> lock(bucket->mutex_);

		std::for_each(bucket->contained_objects_.begin(), bucket->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
			if(region->within_extent(e->QueryInterface<TransformComponentInterface>("Transform")->position()))
			{
				e->QueryInterface<TransformComponentInterface>("Transform")->insert_region(region);
				region->on_appear(e);
			}
		});

		{
			boost::upgrade_to_unique_lock<boost::shared_mutex> unique(lock);
			bucket->contained_regions_.insert(region);
		}
	});
}

void universe_container_component::remove(std::shared_ptr<RegionInterface> region)
{
	std::set<size_t> buckets = buckets_within_verts_(region->verts());
	std::for_each(buckets.begin(), buckets.end(), [&, this] (size_t id) {
		std::shared_ptr<universe_bucket> bucket = buckets_[id];
		boost::upgrade_lock<boost::shared_mutex> lock(bucket->mutex_);
		{
			boost::upgrade_to_unique_lock<boost::shared_mutex> unique(lock);
			bucket->contained_regions_.erase(region);
		}

		std::for_each(bucket->contained_objects_.begin(), bucket->contained_objects_.end(), [&] (std::shared_ptr<Entity> e) {
			if(region->within_extent(e->QueryInterface<TransformComponentInterface>("Transform")->position()))
			{
				region->on_disappear(e);
				e->QueryInterface<TransformComponentInterface>("Transform")->remove_region(region);
			}
		});

	});
}

std::set<size_t> universe_container_component::buckets_within_verts_(const std::vector<glm::vec3>& region)	
{
	glm::vec3 max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	glm::vec3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

	std::for_each(region.begin(), region.end(), [&] (const glm::vec3& vert) {
		if(max.x < vert.x)
			max.x = vert.x;
		if(max.y < vert.y)
			max.y = vert.y;
		if(max.z < vert.z)
			max.z = vert.z;

		if(min.x > vert.x)
			min.x = vert.x;
		if(min.y > vert.y)
			min.y = vert.y;
		if(min.z > vert.z)
			min.z = vert.z;
	});

	size_t min_x = (min.x + half_map_width_) / bucket_width_;
	size_t max_x = (max.x + half_map_width_) / bucket_width_;

	size_t min_y =(is_3D_) ? (min.y + half_map_width_) / bucket_width_ : 0;
	size_t max_y =(is_3D_) ? (max.y + half_map_width_) / bucket_width_ : 0;

	size_t min_z = (min.z + half_map_width_) / bucket_width_;
	size_t max_z = (max.z + half_map_width_) / bucket_width_;

	std::set<size_t> result_set;
	for(size_t y = min_y; y <= max_y; ++y)
	{
		for(size_t x = min_x; x <= max_x; ++x)
		{
			for(size_t z = min_z; z <= max_z; ++z)
			{
				result_set.insert(x + y*buckets_sq + z*buckets_per_row_);
			}
		}
	}
	return result_set;
}

void universe_container_component::generate_relevant_buckets_(int depth, std::shared_ptr<universe_bucket> bucket)
{
	int b_id = bucket->id_;
	size_t low_x, high_x, low_y, high_y, low_z, high_z;

	//Find current values for x and z and temporarily keep them in the low variables.
	low_x = b_id % buckets_per_row_;
	low_z = (b_id - low_x) /  buckets_per_row_;

	if(is_3D_)
	{
		//Find the current value for y
		low_y = low_z / buckets_per_row_;

		//Adjust depth levels
		high_y = low_y + depth;
		low_y -= depth;

		//Clean up the y values.
		if(low_y < 0)
			low_y = 0;
		if(high_y >= buckets_per_row_)
			high_y = buckets_per_row_-1;
	}
	else
	{
		low_y = 0;
		high_y = 0;
	}

	//Adjust the depth levels for x and z
	high_x = low_x + depth;
	high_z = low_z + depth;

	low_x -= depth;
	low_z -= depth;

	//Clean up the current values.
	if(low_x < 0)
		low_x = 0;

	if(low_z < 0)
		low_z = 0;

	if(high_x >= buckets_per_row_)
		high_x = buckets_per_row_-1;

	if(high_z >= buckets_per_row_)
		high_z = buckets_per_row_-1;

	//Least run loop on the outside loop on the outside.
	for(unsigned int y = low_y; y <= high_y; ++y)
	{
		for(unsigned int x = low_x; x <= high_x; ++x)
		{	
			for(unsigned int z = low_z; z <= high_z; ++z)
			{
				bucket->relevant_buckets_.insert(buckets_[x + y*buckets_sq + z*buckets_per_row_]);
			}
		}
	}
}

size_t universe_container_component::relevant_bucket_(const glm::vec3& pos)
{
	size_t x = (pos.x + half_map_width_) / bucket_width_;
	size_t y = (is_3D_) ? (pos.y + half_map_width_) / bucket_width_ : 0;
	size_t z = (pos.z + half_map_width_) / bucket_width_;

	return x + y*buckets_sq + z*buckets_per_row_;
}

bool universe_container_component::bucketcomp::operator()(const std::shared_ptr<universe_bucket>& lhs, const std::shared_ptr<universe_bucket>& rhs)
{
	return lhs->id_ < rhs->id_;
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