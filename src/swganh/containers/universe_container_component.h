#ifndef UNIVERSE_CONTAINER_COMPONENT_H
#define UNIVERSE_CONTAINER_COMPONENT_H

#include "container_component_interface.h"
#include <swganh/regions/region_interface.h>

#include <memory>
#include <vector>
#include <set>

#include <boost/thread/shared_mutex.hpp>

namespace anh
{
namespace component
{
	class Entity;
};
};

namespace swganh
{
	namespace regions
	{
	class RegionInterface;
	};

namespace containers
{
	class universe_container_component : public swganh::containers::ContainerComponentInterface, public std::enable_shared_from_this<universe_container_component>
	{
	public:

		class universe_bucket;

		struct bucketcomp
		{
			virtual bool operator()(const std::shared_ptr<universe_bucket>& lhs, const std::shared_ptr<universe_bucket>& rhs);
		};

		class universe_bucket
		{
			public:

				universe_bucket(size_t id) : id_(id) { }

				void insert(std::shared_ptr<anh::component::Entity> e) { contained_objects_.insert(e); }
				void remove(std::shared_ptr<anh::component::Entity> e) { contained_objects_.erase(e); }

				std::set<std::shared_ptr<universe_bucket>, bucketcomp> relevant_buckets_;
				std::set<std::shared_ptr<anh::component::Entity>> contained_objects_;
				std::set<std::shared_ptr<swganh::regions::RegionInterface>> contained_regions_;

				boost::shared_mutex mutex_;
				size_t id_;
		};

		universe_container_component(float map_size, float bucket_size, float viewing_range, bool _3_dimensional_);

		//Region Methods
		void insert(std::shared_ptr<swganh::regions::RegionInterface> region);
		void remove(std::shared_ptr<swganh::regions::RegionInterface> region);

		//View Methods
		virtual bool has_entity(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what);
		virtual bool contained_objects(std::shared_ptr<anh::component::Entity> who, bool causes_populate, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth=0, bool top_down=true);
		virtual std::shared_ptr<anh::component::Entity> entity_in_slot(std::shared_ptr<anh::component::Entity> who,std::string slot_name);
		virtual std::set<std::shared_ptr<anh::component::Entity>> aware_entities(std::shared_ptr<anh::component::Entity> caller_hint);

		//Modification Methods
		virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion=false);
		virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, unsigned int id, bool force_insertion=false) { return false; }
		virtual bool remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal=false);
		virtual bool transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion=false, bool force_removal=false);
		
		//Notification Methods
		virtual void make_aware(std::shared_ptr<anh::component::Entity> what);

		virtual void state_update(std::shared_ptr<anh::component::Entity> what, const glm::vec3& oldpos, const glm::vec3& newpos, const glm::quat& rot);

		virtual void make_unaware(std::shared_ptr<anh::component::Entity> what);

		//Permission Methods
		virtual bool permissions_can_view(std::shared_ptr<anh::component::Entity> who) { return true; }
		virtual bool permissions_can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return true; }
		virtual bool permissions_can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, const std::set<anh::HashString>& arrangement_to_use) { return false; }
		virtual bool permissions_can_remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return true; }

		virtual bool permissions_grant_view(std::shared_ptr<anh::component::Entity> who) { return false; }
		virtual bool permissions_revoke_view(std::shared_ptr<anh::component::Entity> who) { return false; }
		virtual bool permissions_grant_insert(std::shared_ptr<anh::component::Entity> who) { return false; }
		virtual bool permissions_revoke_insert(std::shared_ptr<anh::component::Entity> who) { return false; }
		virtual bool permissions_grant_removal(std::shared_ptr<anh::component::Entity> who) { return false; }
		virtual bool permissions_revoke_removal(std::shared_ptr<anh::component::Entity> who) { return false; }

		virtual bool permissions_grant_view(std::string argument) { return false; }
		virtual bool permissions_revoke_view(std::string argument) { return false; }
		virtual bool permissions_grant_insert(std::string argument) { return false; }
		virtual bool permissions_revoke_insert(std::string argument) { return false; }
		virtual bool permissions_grant_removal(std::string argument) { return false; }
		virtual bool permissions_revoke_removal(std::string argument) { return false; }

		//Size/Capacity Methods
		virtual bool empty();
		virtual bool full();

		virtual size_t size();
		virtual size_t capacity();
		virtual bool capacity(size_t new_capacity);
	
		virtual bool collect_garbage() { return false; }
		virtual bool is_populated() { return true; }

		virtual void persist() {}
		virtual void populate() {}

		virtual bool intrl_insert_(std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> old_container);

	private:
		std::set<size_t> buckets_within_verts_(const std::vector<glm::vec3>& region);

		void generate_relevant_buckets_(int depth, std::shared_ptr<universe_bucket> bucket);
		size_t relevant_bucket_(const glm::vec3& pos);

		float half_map_width_;
		float bucket_width_;
		float viewing_range_;
		size_t buckets_per_row_;
		size_t buckets_sq;
		bool is_3D_;

		std::vector<std::shared_ptr<universe_bucket>> buckets_;
	};
};
};

#endif