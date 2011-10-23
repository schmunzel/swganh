#include <glog/logging.h>

#include "swganh/object/object.h"
#include "swganh/spatial index/spatial_bucket.h"

using namespace std;
using namespace swganh;
using namespace simulation;
using namespace spatialindex;
using namespace spatialbucket;

Bucket::Bucket(uint32_t id)	{
	bucket_id = id;
}

void Bucket::GetContent(ObjectListType* list, uint32_t type)	{
	
	if(type == 0)    {
        assert(false && "Bucket::GetContent type must NOT be 0");
    }

    DLOG(INFO) << "Bucket::GetContent :: Bucket " << bucket_id << " list-type : " << type;

    //make a copy of the list !!!!
    //splice removes the entries!!!
    ObjectListType listCopy;
	ObjectListType::iterator it = list->begin();

    if(type&&Bucket_Objects)    {
        listCopy = object_list;
        list->splice(it, listCopy);
    }

    if(type&&Bucket_Players)    {
        listCopy = player_list;
        list->splice(it, listCopy);
    }

    if(type&&Bucket_Creatures)    {
        listCopy = creature_list;
        list->splice(it, listCopy);
    }

}

void Bucket::GetPlayers(ObjectListType* list)	{
	
    DLOG(INFO) << "Bucket::GetPlayers :: Bucket " << bucket_id;

    //make a copy of the list !!!!
    //splice removes the entries!!!
    ObjectListType listCopy;
	ObjectListType::iterator it = list->begin();

    listCopy = player_list;
    list->splice(it, listCopy);

}

void Bucket::RemoveContent(const uint64_t object_id, uint32_t type)	{

	DLOG(INFO) << "Bucket::RemoveContent " << object_id << " out of Bucket " << bucket_id;

	ObjectListType *list;

	if(type&&Bucket_Objects)    {
        list = &object_list;
    }

    if(type&&Bucket_Players)    {
        list = &player_list;
    }

    if(type&&Bucket_Creatures)    {
        list = &creature_list;
    }

	for(ObjectListType::iterator remove_it = list->begin(); remove_it != list->end(); remove_it++)    {
        if((*remove_it)->GetObjectId() == object_id)      {
            list->erase(remove_it);
            break;
        }
    }

}

void Bucket::RemovePlayer(const uint64_t object_id)	{

	DLOG(INFO) << "Bucket::RemovePlayer " << object_id << " out of Bucket " << bucket_id;

	ObjectListType *list;	
    
    list = &player_list;
    
	for(ObjectListType::iterator remove_it = list->begin(); remove_it != list->end(); remove_it++)    {
        if((*remove_it)->GetObjectId() == object_id)      {
            list->erase(remove_it);
            break;
        }
    }

}


void Bucket::AddContent(const std::shared_ptr<swganh::object::Object>& object, uint32_t type)	{

	DLOG(INFO) << "Bucket::AddContent " << object->GetObjectId() << " to Bucket " << bucket_id;

	ObjectListType *list;

	if(type&&Bucket_Objects)    {
        list = &object_list;
    }

    if(type&&Bucket_Players)    {
        list = &player_list;
    }

    if(type&&Bucket_Creatures)    {
        list = &creature_list;
    }

	list->push_back(object);

}

void Bucket::AddPlayer(const std::shared_ptr<swganh::object::Object>& object)	{

	DLOG(INFO) << "Bucket::AddPlayer " << object->GetObjectId() << " to Bucket " << bucket_id;

	ObjectListType *list;	
    
    list = &player_list;
    
	list->push_back(object);

}