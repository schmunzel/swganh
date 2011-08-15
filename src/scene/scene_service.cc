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

#include "scene/scene_service.h"

#include <boost/lexical_cast.hpp>

#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/sqlstring.h>

#include <glog/logging.h>

#include <iomanip>

#include "anh/crc.h"

#ifdef WIN32
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#include "anh/app/kernel_interface.h"
#include "anh/database/database_manager.h"
#include "anh/event_dispatcher/event_dispatcher.h"

#include "anh/network/soe/session.h"
#include "anh/network/soe/server_interface.h"

#include "anh/service/service_manager.h"

#include "swganh/scene/messages/cmd_start_scene.h"
#include "swganh/scene/messages/scene_create_object_by_crc.h"
#include "swganh/scene/messages/scene_end_baselines.h"

#include "swganh/connection/base_connection_service.h"
#include "swganh/connection/connection_client.h"
#include "swganh/connection/messages/heart_beat.h"

#include "swganh/containers/universe_container_component.h"
#include "swganh/transform/transform_component.h"

using namespace swganh::scene;
using namespace swganh::scene::messages;
using namespace swganh::connection::messages;
using namespace scene;
using namespace anh;
using namespace app;
using namespace component;
using namespace event_dispatcher;
using namespace database;
using namespace std;
using namespace swganh::connection;

SceneService::SceneService(shared_ptr<KernelInterface> kernel, const std::string& name) 
    : BaseSceneService(kernel) {
    scene_name(name);
    entity_manager_ = make_shared<EntityManager>();
    entity_builder_ = make_shared<EntityBuilder>(entity_manager_);
    entity_builder_->Init("templates");
    baseline_service_ = static_pointer_cast<baseline::BaselineService>(kernel->GetServiceManager()->GetService("BaselineService"));
}

SceneService::~SceneService() {}

service::ServiceDescription SceneService::GetServiceDescription() {
    service::ServiceDescription service_description(
        scene_name() + " Scene Service",
        "scene",
        "0.1",
        "127.0.0.1", 
        0, 
        0, 
        0);

    return service_description;
}

void SceneService::onStart() 
{
	RegisterComponentCreators();
	CreateScene();
}

void SceneService::onStop() {}

void SceneService::subscribe() {
    auto connection_service = std::static_pointer_cast<BaseConnectionService>(kernel()->GetServiceManager()->GetService("ConnectionService"));
    
    kernel()->GetEventDispatcher()->subscribe("SelectCharacterLogin",[this](shared_ptr<EventInterface> incoming_event) ->bool {
        auto select_character = static_pointer_cast<BasicEvent<swganh::character::CharacterLoginData>>(incoming_event);
        AddEntityClient_(select_character->character_id, select_character->client);
        return AddPlayerToScene(*select_character);
    });

    kernel()->GetEventDispatcher()->subscribe("NetworkSessionRemoved", [this] (shared_ptr<EventInterface> incoming_event) -> bool {
        auto session_removed = std::static_pointer_cast<anh::event_dispatcher::BasicEvent<anh::network::soe::SessionData>>(incoming_event);
        
        EntityClientMap& client_map = entity_clients();

        auto find_it = std::find_if(
            client_map.begin(), 
            client_map.end(), 
            [session_removed] (swganh::scene::BaseSceneService::EntityClientMap::value_type& entity_client) 
        {
            return entity_client.second->session == session_removed->session;
        });
        
        if (find_it != client_map.end()){
            RemoveEntityClient_((*find_it).first);
        }

        return true;
    });
}

void SceneService::DescribeConfigOptions(boost::program_options::options_description& description) {}

void SceneService::RegisterComponentCreators()
{
	entity_builder_->RegisterCreator("universe_container", [] (const EntityId&) -> std::shared_ptr<swganh::containers::universe_container_component> {
		return std::make_shared<swganh::containers::universe_container_component>(16384, 128, 128, false);
	});

	entity_builder_->RegisterCreator("transform", [] (const EntityId&) -> std::shared_ptr<swganh::transform::TransformComponent> {
		return std::make_shared<swganh::transform::TransformComponent>();
	});
}

std::shared_ptr<swganh::connection::ConnectionClient> SceneService::getConnectionClient(uint64_t entity_id)
{
    EntityClientMap& client_map = entity_clients();

    auto find_it = std::find_if(
        client_map.begin(), 
        client_map.end(), 
        [entity_id] (EntityClientMap::value_type& entity_client) 
    {
        return entity_client.first == entity_id;
    });

    if (find_it != client_map.end()){
        return (*find_it).second;
    }
    else
        return nullptr;
}

bool SceneService::AddEntityClient_(uint64_t entity_id, std::shared_ptr<swganh::connection::ConnectionClient> client) {
    EntityClientMap& client_map = entity_clients();

    auto find_it = std::find_if(
        client_map.begin(), 
        client_map.end(), 
        [entity_id] (EntityClientMap::value_type& entity_client) 
    {
        return entity_client.first == entity_id;
    });
    // it's already here
    // @TODO: deal with this
    if (find_it != client_map.end()) {
        return false;
    }
    
    DLOG(WARNING) << "Adding entity to connection client:" << client->session->connection_id();

    EntityClientMap::accessor a;
    client_map.insert(a, entity_id);
    a->second = client;

    DLOG(WARNING) << GetServiceDescription().name() << " #" << GetServiceDescription().id() <<  " currently has ("<< client_map.size() << ") entities mapped to clients";
    
    return true;
}
bool SceneService::RemoveEntityClient_(uint64_t entity_id)
{
    DLOG(WARNING) << "Removing entity client: " << entity_id;
    return entity_clients().erase(entity_id);
}
bool SceneService::CreateScene() 
{
	entity_builder_->BuildEntity(nullptr, 0, "universe", "", [] () -> std::uint64_t { return 0; });

    return true;
}

bool SceneService::AddPlayerToScene(swganh::character::CharacterLoginData character)
{
    // create our player
	std::uint64_t i = character.character_id;
    auto entity_errors = entity_builder()->BuildEntity(0, "player", "anh.Player", [&] () -> std::uint64_t {return i++;});
    auto entity = entity_manager()->GetEntity(character.character_id);
    
    CmdStartScene start_scene;
    // @TODO: Replace with configurable value
    start_scene.ignore_layout = 0;
    start_scene.character_id = entity->id();
    start_scene.terrain_map = character.terrain_map;
    start_scene.position = character.position;
    start_scene.shared_race_template = "object/creature/player/shared_" + character.race + "_" + character.gender + ".iff";
    start_scene.galaxy_time = service_directory()->galaxy().GetGalaxyTimeInMilliseconds();
        
    character.client->session->SendMessage(start_scene);
    SceneCreateObjectByCrc scene_object;
    scene_object.object_id = entity->id();
    scene_object.orientation = character.orientation;
    scene_object.position = character.position;
    scene_object.object_crc = anh::memcrc(character.race_template);
    // @TODO: Replace with configurable value
    scene_object.byte_flag = 0;
    character.client->session->SendMessage(scene_object);

    // Send Baselines
    auto conn_client = getConnectionClient(character.character_id);
    baseline_service()->send_baselines_self(entity, conn_client);

    return true;
}

bool SceneService::LoadMap(const std::string& map_file) 
{
    // find map
    // use terrain manager to load map
    return true;
}

bool SceneService::LoadSceneEntities()
{
    // based on scene name, load the database information for each entity
    // This will likely just call out to a database provider that will then do the dirty work
    return true;
}

bool SceneService::AddEntityToScene(uint64_t entity_id)
{
    // Build the entity and then pass baselines to relevant areas.
    return true;
}