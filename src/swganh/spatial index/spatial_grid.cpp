#include "swganh/spatial index/spatial_grid.h"
#include "swganh/object/player/player.h"

#include <glog/logging.h>

using namespace std;
using namespace swganh;
using namespace simulation;
using namespace spatialindex;
using namespace spatialgrid;
using namespace spatialbucket;

Spatialgrid::Spatialgrid()	{
	Spatialgrid::Spatialgrid(16400,16400,40,40,1,3);

}

Spatialgrid::Spatialgrid(uint32_t map_width, uint32_t map_height, uint32_t bucket_width, uint32_t	bucket_height, uint32_t chat_range, uint32_t view_range)	{

	grid_width_		= map_width/bucket_width;
	grid_height_	= map_height/bucket_height;
	map_width_		= map_width;
	map_height_		= map_height;
	bucket_width_	= bucket_width;
	bucket_height_	= bucket_height;

    view_range_		= view_range;
    chat_range_		= chat_range_;


    uint32_t x = 0, i = 0, j = 0;

    // Setup the lookup array...

    for (x = 0; x <= grid_width_; x++) {
        for (j = 0; j <= grid_height_; j++) {
            zmap_lookup[x][j] = i;
            Bucket* bucket = new Bucket(i);
            ZMapCells.insert(std::make_pair(i,bucket));
            i++;

        }
    }
}

uint32_t Spatialgrid::AddContent(std::shared_ptr<swganh::object::Object>& object, const messages::UpdateTransformMessage& transform_update)
{

	
    uint32_t finalBucket = _getCellId(transform_update.position.x,transform_update.position.z);

    //DLOG(INFO) << "zmap::AddObject :: " << newObject->getId() << " bucket " << finalBucket<< " ";

    if(!GetCellValidFlag(finalBucket))    {
        //something fishy here
        assert(false && "Spatialgrid::AddContent :: couldnt find fitting bucket :(");
        return 0xffffffff;
    }

    //already in there
    if(object->GetBucketId() == finalBucket)    {
		DLOG(INFO) << "SpatialGrid::AddContent :: " << object->GetObjectId() << " Bucket " << finalBucket<< " Object was already in the Bucket";
        return finalBucket;
    }

    object->SetBucketId(finalBucket);

	BucketMap_t::iterator mapIt = ZMapCells.find(finalBucket);

	switch(object->GetType())
    {
		
	case 0x504c4159: //player I know this is evil
    {
		(*mapIt).second->AddContent(object,Bucket_Players);
    }
    break;

    case 0x4352454F://creatures
    {
        (*mapIt).second->AddContent(object,Bucket_Creatures);
    }
    break;

    default: //all static Objects
    {
        (*mapIt).second->AddContent(object,Bucket_Objects);
    }
    break;
    }

    return finalBucket;
}

void Spatialgrid::RemoveContent(std::shared_ptr<swganh::object::Object>& object)	{

	uint32_t cellId = object->GetBucketId();

    if(!GetCellValidFlag(cellId))
    {
        DLOG(INFO) << "Spatialgrid::RemoveContent :: Bucket " << cellId << " NOT valid";
        return;
    }

	DLOG(INFO) << "Spatialgrid::RemoveContent :: " << object->GetObjectId() << " Bucket " << cellId;

    std::map<uint32_t, Bucket*>::iterator it = ZMapCells.find(cellId);

    switch(object->GetType())
    {
		
	case 0x504c4159: //player I know this is evil
    {
		(*it).second->RemovePlayer(object->GetObjectId());
    }
    break;

    case 0x4352454F://creatures
    {
        (*it).second->RemoveContent(object->GetObjectId(),Bucket_Creatures);
    }
    break;

    default: //all static Objects
    {
        (*it).second->RemoveContent(object->GetObjectId(),Bucket_Objects);
    }
    break;
    }

    //make sure we can use the mGridBucket to determine what bucket we *are* in
    //so we do not have to search the list on insert
    object->SetBucketId(0xffffffff);

	
	//remove out of any regions we might be in
	//We need to check which subregions to leave
/*	Uint64Set*				region_set	= &removeObject->zmapSubCells;
    Uint64Set::iterator		set_it		= region_set->begin();

    while(set_it != region_set->end())    {
        auto region = findRegion(*set_it);

        if (!region) {
		    region_set->erase(set_it++);		
        }

		region->onObjectLeave(removeObject);
		region_set->erase(set_it++);	
    }
	*/
}

void Spatialgrid::UpdateObject(std::shared_ptr<swganh::object::Object>& object)
{
    glm::vec3   position;

    //cater for players in cells
	std::shared_ptr<swganh::object::Object> parent = object->GetContainer();
    if (parent != nullptr)	{
        position = object->GetWorldPosition();
    }
    else	{
        position = object->GetPosition();
    }

    uint32_t	newBucket	= getCellId(position.x, position.z);
    uint32_t	oldBucket	= object->GetBucketId();

	if(!GetCellValidFlag(newBucket))    {
        //something fishy here
        assert(false && "Spatialgrid::AddContent :: couldnt find fitting bucket :(");
    }

    //no need for an update
    if(newBucket == oldBucket)	{
		DLOG(INFO) << "Spatialgrid::UpdateObject :: the new Bucket ID " << newBucket << " is the same as the old one";
        return;
    }

	std::map<uint32_t, Bucket*>::iterator old_it = ZMapCells.find(oldBucket);
	std::map<uint32_t, Bucket*>::iterator new_it = ZMapCells.find(oldBucket);

    switch(object->GetType())    {
		
	//players
	case 0x504c4159:   {
		(*old_it).second->RemovePlayer(object->GetObjectId());
		(*new_it).second->AddPlayer(object);
    }
    break;

	//creatures
    case 0x4352454F:    {
        (*old_it).second->RemoveContent(object->GetObjectId(),Bucket_Creatures);
		(*new_it).second->AddContent(object,Bucket_Creatures);
    }
    break;

	//all static Objects
    default:    {
        (*old_it).second->RemoveContent(object->GetObjectId(),Bucket_Objects);
		(*new_it).second->AddContent(object,Bucket_Creatures);
    }
    break;
    }

    //put into new bucket
    object->SetBucketId(newBucket);

}


bool Spatialgrid::GetCellValidFlag(uint32_t bucket)
{
    if(bucket > (grid_width_*grid_height_))
        return false;
    return true;
}