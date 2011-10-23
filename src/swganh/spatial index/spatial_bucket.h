#ifndef SWGANH_SIMULATION_SPATIAL_BUCKET_H_
#define SWGANH_SIMULATION_SPATIAL_BUCKET_H_

#include <cstdint>
#include <map>
#include <list>
#include <memory>
#include <set>
#include <string>

#include <boost/noncopyable.hpp>

namespace swganh {
namespace object {
    class Object;
}}  // namespace swganh::object

namespace swganh {
namespace simulation {
namespace spatialindex {
namespace spatialbucket {

	typedef std::shared_ptr<swganh::object::Object>			Object;
	typedef std::list<Object>								ObjectListType;
	//typedef std::set<swganh::object::Object>				ObjectSet;
	typedef std::multimap<uint32_t, std::shared_ptr<Object>>	SubCellMap;

	enum BucketType {
		Bucket_Creatures = 1,
		Bucket_Objects	 = 2,
		Bucket_Players	 = 4
	};

    class Bucket : boost::noncopyable
    {
    public:
        explicit Bucket(uint32_t id);
        
        uint32_t			GetBucketId() const;
        
	

		/** 
		* Bucket::GetContent(ObjectListType* list, uint32_t type) 
		* will splice the content of the buckets lists (as indicated by type)
		* into list. List has to be created by the calling function and the content 
		* after useage has to be disposed by the calling function
		*/
		void GetContent(ObjectListType* list, uint32_t type);
		
		/** 
		* Bucket::GetContent(ObjectListType* list, uint32_t type) 
		* will splice the content of the player list 
		* into list. List has to be created by the calling function and the content 
		* after useage has to be disposed by the calling function
		*/
		void GetPlayers(ObjectListType* list);

		/** 
		* Bucket::RemoveContent(uint64_t object_id, uint32_t type) 
		* will remove the object reference out of the list provided by type 
		*/
        void RemoveContent(const uint64_t object_id, uint32_t type);

		/** 
		* Bucket::RemovePlayer(uint64_t object_id) 
		* will remove the player reference out of the playerlist 
		*/
		void RemovePlayer(const uint64_t object_id);


		/** 
		* Bucket::AddContent(uint64_t object_id, uint32_t type) 
		* will add the object reference to the list provided by type 
		*/
        void AddContent(const std::shared_ptr<swganh::object::Object>& object, uint32_t type);

		/** 
		* Bucket::AddPlayer(uint64_t object_id) 
		* will add the player reference to the playerlist 
		*/
		void AddPlayer(const std::shared_ptr<swganh::object::Object>& object);

    private:
        
		ObjectListType      object_list;
		ObjectListType      creature_list;
		ObjectListType      player_list;
		ObjectListType		subCell_list;

		uint32_t			bucket_id;
  
    };

}}}}  // namespace swganh::simulation::spatialindex::spatialbucket 

#endif  // SWGANH_SIMULATION_SPATIAL_BUCKET_H_
