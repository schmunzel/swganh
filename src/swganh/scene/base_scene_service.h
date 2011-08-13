/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SWGANH_SCENE_BASE_SCENE_SERVICE_H_
#define SWGANH_SCENE_BASE_SCENE_SERVICE_H_

#include <cstdint>
#include <tuple>
#include <vector>

#include <tbb/concurrent_hash_map.h>

#include "swganh/base/base_service.h"

namespace anh {
namespace app {
class KernelInterface;
}}  // namespace anh::app
namespace swganh {
namespace connection {
    struct ConnectionClient;
} // namespace connection
namespace character {
struct CharacterLoginData;
}}  // namespace swganh::character

namespace swganh {
namespace scene {

/*
*  @brief Scene Service manages the game space for a specific area (planet or instance generally)
*/
class BaseSceneService : public swganh::base::BaseService {
public:    
    typedef tbb::concurrent_hash_map<
        uint64_t, 
        std::shared_ptr<swganh::connection::ConnectionClient>
    > EntityClientMap;

    explicit BaseSceneService(std::shared_ptr<anh::app::KernelInterface> kernel) 
        : swganh::base::BaseService(kernel) {}

    // API
    /**
    *  @brief Loadup Map for Scene
    */
    virtual bool LoadMap(const std::string& map_file) = 0;
    /**
    *  @brief Loads entities into the Scene
    */
    virtual bool LoadSceneEntities() = 0;

    /**
    *  @brief adds the player to the scene
    */
    virtual bool AddPlayerToScene(swganh::character::CharacterLoginData data) = 0;

    /**
    *  @brief adds the entity to the scene
    */
    virtual bool AddEntityToScene(uint64_t entity_id) = 0;

    /**
    *  @brief Gets the Scene Name
    */
    const std::string& scene_name() { return scene_name_; }
    void scene_name(const std::string& name) { scene_name_ = name; }

    /**
    *  @brief Returns if the Scene is a planet
    */
    bool IsPlanet() { return is_planet_; }
    

    EntityClientMap& entity_clients() { return entity_clients_; }
private:
    std::string scene_name_;
    bool is_planet_;
    EntityClientMap entity_clients_;
    BaseSceneService();
};

}}  // namespace swganh::scene

#endif  // SWGANH_SCENE_BASE_SCENE_SERVICE_H_

