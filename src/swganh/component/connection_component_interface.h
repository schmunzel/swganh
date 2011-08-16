/*
---------------------------------------------------------------------------------------
This source file is part of SWG:ANH (Star Wars Galaxies - A New Hope - Server Emulator)

For more information, visit http://www.swganh.com

Copyright (c) 2006 - 2011 The SWG:ANH Team
---------------------------------------------------------------------------------------
Use of this source code is governed by the GPL v3 license that can be found
in the COPYING file or at http://www.gnu.org/licenses/gpl-3.0.html

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,a
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
---------------------------------------------------------------------------------------
*/
#ifndef SWGANH_COMPONENT_CONNECTION_COMPONENT_INTERFACE_H
#define SWGANH_COMPONENT_CONNECTION_COMPONENT_INTERFACE_H

#include <anh/component/component_interface.h>
#include <anh/component/base_component.h>
#include <anh/network/soe/session.h>

#include <boost/flyweight.hpp>

namespace swganh { 
namespace component {

class NullConnectionComponent;

class ConnectionComponentInterface : public anh::component::BaseComponent {
public:
    ConnectionComponentInterface(const anh::component::ComponentType& type)
        : anh::component::BaseComponent("connection", type) { }
    
    virtual std::shared_ptr<anh::network::soe::Session> session() = 0;
    virtual void session(std::shared_ptr<anh::network::soe::Session> session_in) = 0;
    virtual uint64_t player_id() = 0;
    virtual void player_id(uint64_t playerid) = 0;
    virtual uint32_t account_id() = 0;
    virtual void account_id(uint32_t accountid) = 0;

    static std::shared_ptr<NullConnectionComponent> NullComponent;
};
class NullConnectionComponent : public ConnectionComponentInterface {
public:
    NullConnectionComponent()
        : ConnectionComponentInterface("NullConnection") { }
    virtual std::shared_ptr<anh::network::soe::Session> session() { return session_; }
    virtual void session(std::shared_ptr<anh::network::soe::Session> session_in) { session_ = session_in; }
    virtual uint64_t player_id() { return player_id_; }
    virtual void player_id(uint64_t playerid) { player_id_ = playerid; }
    uint32_t account_id() { return account_id_; }
    virtual void account_id(uint32_t accountid) { account_id_ = accountid; }
private:
    std::shared_ptr<anh::network::soe::Session> session_;
    uint64_t player_id_;
    uint32_t account_id_;
};
}} //swganh::component
#endif //SWGANH_COMPONENT_CONNECTION_COMPONENT_INTERFACE_H