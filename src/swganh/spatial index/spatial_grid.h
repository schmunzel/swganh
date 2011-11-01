#ifndef SWGANH_SIMULATION_SPATIAL_GRID_H_
#define SWGANH_SIMULATION_SPATIAL_GRID_H_

#include <cstdint>
#include <map>
#include <list>
#include <memory>
#include <set>
#include <string>

#include <boost/noncopyable.hpp>

#include "swganh/spatial index/spatial_bucket.h"
#include "swganh/messages/update_transform_message.h"
#include "swganh/object/object.h"

namespace swganh {
namespace object {
    class Object;
}}  // namespace swganh::object

namespace swganh {
namespace simulation {
namespace spatialindex {
namespace spatialgrid {

#define GRIDWIDTH 410
#define GRIDHEIGHT 410

#define MAPWIDTH 16400
#define MAPHEIGHT 16400

#define VIEWRANGE 3
#define CHATRANGE 1

	typedef std::map<uint32_t, spatialbucket::Bucket*> BucketMap_t;

	class Spatialgrid {
public:
	// Contructor & Destructor
	Spatialgrid(uint32_t map_width, uint32_t map_height, uint32_t bucket_width, uint32_t	bucket_height, uint32_t chat_range_, uint32_t view_range_);
	Spatialgrid();
	~Spatialgrid();
        
	//regioncode look at later
	/*
	void updateRegions(Object* object);
    
	void addRegion(std::shared_ptr<RegionObject> region);
	std::shared_ptr<RegionObject> findRegion(uint64_t region_id);

	*/

	/* Add an object to zmap - returns the cell
	*
	*/
	uint32_t				AddContent(std::shared_ptr<swganh::object::Object>& object, const messages::UpdateTransformMessage& transform_update);

	/* Remove an object to zmap - returns the cell
	*
	*/
	void					RemoveContent(std::shared_ptr<swganh::object::Object>& object);
	
	/* Checks whether a given Bucket exists
	*
	*/
	bool					GetCellValidFlag(uint32_t bucket);
	
	
	/* Updates a given Objects bucket
	*
	*/
	void					UpdateObject(std::shared_ptr<swganh::object::Object>& object);

	/*
	//bool		checkPlayersNearby(Object* updateObject);

	


	bool				isObjectInRegion(Object* object, uint64 regionid);
	void				RemoveRegion(uint64 regionId);
    	

	//Get the contents of current cell of the player, looked up by CellID
	void				GetCellContents(uint32 CellID, ObjectListType* list, uint32 type);//gets contents based on type enum
	void				GetPlayerCellContents(uint32 CellID, ObjectListType* list);
	
	//=====================================================
	//row
	void				GetPlayerGridContentsListRow(uint32 CellID, ObjectListType* list);
	void				GetGridContentsListRow(uint32 CellID, ObjectListType* list, uint32 type);
	
	
	//=====================================================
	//(viewRange*2)-1 to accomodate for diametral movement
	void				GetGridContentsListRowLeft(uint32 CellID, ObjectListType* list, uint32 type);
	void				GetPlayerGridContentsListRowLeft(uint32 CellID, ObjectListType* list);

	void				GetGridContentsListRowRight(uint32 CellID, ObjectListType* list, uint32 type);
	void				GetPlayerGridContentsListRowRight(uint32 CellID, ObjectListType* list);
		
	void				GetGridContentsListColumn(uint32 CellID, ObjectListType* list, uint32 type);
	void				GetPlayerGridContentsListColumn(uint32 CellID, ObjectListType* list);

	//=====================================================
	//(viewRange*2)-1 to accomodate for diametral movement
	void				GetPlayerGridContentsListColumnDown(uint32 CellID, ObjectListType* list);
	void				GetGridContentsListColumnDown(uint32 CellID, ObjectListType* list, uint32 type);
	

	void				GetPlayerGridContentsListColumnUp(uint32 CellID, ObjectListType* list);
	void				GetGridContentsListColumnUp(uint32 CellID, ObjectListType* list, uint32 type);
	

	//Get the contents of chatrange cells
	void				GetChatRangeCellContents(uint32 CellID, ObjectListType* list);

	void				GetViewingRangeCellContents(uint32 CellID, ObjectListType* list, uint32 type);
	void				GetPlayerViewingRangeCellContents(uint32 CellID, ObjectListType* list);
	
	void				GetCustomRangeCellContents(uint32 CellID, uint32 range, ObjectListType* list, uint32 type);
	
	*/

	uint32_t			getCellId(float x, float z){return _getCellId(x, z);}


private:

	/* get the ID of the Bucket associated with the given coordinates
	*
	*/
	uint32_t		_getCellId(float x, float z);
	
	//bool		isObjectInRegionBoundary_(Object* object, std::shared_ptr<RegionObject> region);

	
	//std::map<uint32_t, spatialbucket::ObjectListType>		MapHandler;
	
	BucketMap_t		ZMapCells;

	uint32_t		zmap_lookup[GRIDWIDTH+1][GRIDHEIGHT+1]; // one extra for protection
		

	uint32_t		mCurrentSubCellID;
	uint32_t		viewRange;
	uint32_t		chatRange;


	uint32_t	grid_width_;
	uint32_t	grid_height_;
	uint32_t	map_width_;
	uint32_t	map_height_;
	uint32_t	bucket_width_;
	uint32_t	bucket_height_;
	uint32_t	view_range_;// in buckets
	uint32_t	chat_range_;// in buckets

protected:

	//FILE*			ZoneLogs;

	
	//SubCellMap		subCells;


};
#define ZMap	zmap::GetZmap()





	
}}}}  // namespace swganh::simulation::spatialindex::spatialgrid

#endif  // SWGANH_SIMULATION_SPATIAL_GRID_H_
