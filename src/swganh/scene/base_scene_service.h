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
    
class BaseSceneService : public swganh::base::BaseService {
public:    
    typedef tbb::concurrent_hash_map<
        uint64_t, 
        std::shared_ptr<swganh::connection::ConnectionClient>
    > EntityClientMap;

    explicit BaseSceneService(std::shared_ptr<anh::app::KernelInterface> kernel) 
        : swganh::base::BaseService(kernel) {}

    EntityClientMap& entity_clients() { return entity_clients_; }
private:
    EntityClientMap entity_clients_;
    BaseSceneService();
};

}}  // namespace swganh::scene

#endif  // SWGANH_SCENE_BASE_SCENE_SERVICE_H_

