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

#ifndef SCENE_SCENE_SERVICE_H_
#define SCENE_SCENE_SERVICE_H_

#include "swganh/scene/base_scene_service.h"
#include "anh/component/entity_manager.h"
#include "anh/component/entity_builder.h"
#include "baseline/baseline_service.h"
#include "swganh/base/base_service.h"


namespace anh {
namespace app {
class KernelInterface;
}}  // namespace anh::app

namespace anh { namespace database { class DatabaseManagerInterface; } }

namespace scene {
    
class SceneService : public swganh::scene::BaseSceneService {
public:
    SceneService(std::shared_ptr<anh::app::KernelInterface> kernel, const std::string& scene_name);
    ~SceneService();
    
    anh::service::ServiceDescription GetServiceDescription();

    void onStart();
    void onStop();

    void subscribe();

    void DescribeConfigOptions(boost::program_options::options_description& description);

    // SceneService API Methods
    bool AddPlayerToScene(swganh::character::CharacterLoginData data);
    bool LoadMap(const std::string& map_file);
    bool LoadSceneEntities();
    bool AddEntityToScene(uint64_t entity_id);

    std::shared_ptr<swganh::connection::ConnectionClient> getConnectionClient(uint64_t entity_id);
    std::shared_ptr<anh::component::EntityManager> entity_manager() { return entity_manager_; }
    std::shared_ptr<anh::component::EntityBuilder> entity_builder() { return entity_builder_; }
    std::shared_ptr<baseline::BaselineService> baseline_service() { return baseline_service_; }
private:
    bool AddEntityClient_(uint64_t entity_id, std::shared_ptr<swganh::connection::ConnectionClient> connection_client);
    bool RemoveEntityClient_(uint64_t entity_id);
    std::shared_ptr<anh::component::EntityManager> entity_manager_;
    std::shared_ptr<anh::component::EntityBuilder> entity_builder_;
    std::shared_ptr<baseline::BaselineService> baseline_service_;
};

}  // namespace scene

#endif  // SCENE_SCENE_SERVICE_H_
