#ifndef UNIVERSE_CONTAINER_COMPONENT_H
#define UNIVERSE_CONTAINER_COMPONENT_H

#include <api/components/container_component_interface.h>
#include <api/container_permissions_interface.h>
#include <api/region_interface.h>

#include <memory>
#include <vector>
#include <set>

#include <boost/thread/shared_mutex.hpp>

namespace anh
{
namespace component
{
	class Entity;
}
};

namespace container_system
{
	class region_interface;

	class universe_container_component : public anh::api::components::ContainerComponentInterface, public std::enable_shared_from_this<universe_container_component>
	{
	public:

		class universe_bucket
		{
			public:

				universe_bucket() { id = next_id++; }

				void insert(std::shared_ptr<anh::component::Entity> e) { contained_objects_.insert(e); }
				void remove(std::shared_ptr<anh::component::Entity> e) { contained_objects_.erase(e); }

				std::set<std::shared_ptr<universe_bucket>> relevant_buckets_;
				std::set<std::shared_ptr<anh::component::Entity>> contained_objects_;
				std::set<std::shared_ptr<container_system::region_interface>> contained_regions_;

				boost::shared_mutex mutex_;
				int id;

				static int next_id;
		};

		universe_container_component(float map_size, float bucket_size, float viewing_range, bool _3_dimensional_);

		//Region Methods
		void insert(std::shared_ptr<region_interface> region);
		void remove(std::shared_ptr<region_interface> region);

		//View Methods
		virtual bool has_entity(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what);
		virtual bool contained_objects(std::shared_ptr<anh::component::Entity> who, std::function<void(std::shared_ptr<anh::component::Entity>, std::shared_ptr<anh::component::Entity>)> funct, size_t max_depth=0);
		virtual std::shared_ptr<anh::component::Entity> entity_in_slot(std::shared_ptr<anh::component::Entity> who,std::string slot_name);
		virtual std::set<std::shared_ptr<anh::component::Entity>> aware_entities(std::shared_ptr<anh::component::Entity> caller_hint);

		//Modification Methods
		virtual bool insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_insertion=false);
		virtual bool remove(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, bool force_removal=false);
		virtual bool transfer_to(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> recv_container, bool force_insertion=false, bool force_removal=false);
		
		//Notification Methods
		virtual void make_aware(std::shared_ptr<anh::component::Entity> what);
		virtual void universe_container_component::state_update(std::shared_ptr<anh::component::Entity> what, glm::vec3& old);
		virtual void make_unaware(std::shared_ptr<anh::component::Entity> what);

		//Permission Methods
		virtual bool permissions_can_view(std::shared_ptr<anh::component::Entity> who) { return true; }
		virtual bool permissions_can_insert(std::shared_ptr<anh::component::Entity> who, std::shared_ptr<anh::component::Entity> what) { return true; }
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
	
		virtual bool intrl_insert_(std::shared_ptr<anh::component::Entity> what, std::shared_ptr<ContainerComponentInterface> old_container);

	private:
		

		void generate_relevant_buckets_2D_(int depth, std::shared_ptr<universe_bucket> bucket);
		void generate_relevant_buckets_3D_(int depth, std::shared_ptr<universe_bucket> bucket);

		size_t relevant_bucket_(const glm::vec3& pos);
		size_t relevant_bucket_2D_(const glm::vec3& pos);
		size_t relevant_bucket_3D_(const glm::vec3& pos);

		float half_map_width_;
		float bucket_width_;
		float viewing_range_;
		float buckets_per_row_;
		bool is_3D_;

		std::vector<std::shared_ptr<universe_bucket>> buckets_;
	};
};

#endif