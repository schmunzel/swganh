/*
 This file is part of MMOServer. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2010 The SWG:ANH Team

 MMOServer is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 MMOServer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MMOServer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SWGANH_BASE_OBJECT_CONTROLLER_MESSAGE_H
#define SWGANH_BASE_OBJECT_CONTROLLER_MESSAGE_H

#include <cassert>
#include <cstdint>

#include "anh/byte_buffer.h"
#include "anh/utilities.h"

namespace swganh {
namespace base {
      
template<typename T>
struct ObjectControllerMessage {

    static uint32_t opcode() { return 0x80CE5E46; }

    void serialize(anh::ByteBuffer& buffer) const {
        buffer.write(anh::hostToLittle(T::priority()));
        buffer.write(opcode());
        buffer.write<uint32_t>(0x0B); // unknown
        buffer.write(T::sub_opcode());
        buffer.write<uint64_t>(object_id);
        buffer.write(tick_count);
        onSerialize(buffer);
    }
    
    void deserialize(anh::ByteBuffer buffer) {
        uint16_t priority = anh::littleToHost(buffer.read<uint16_t>());
        uint32_t opcode = buffer.read<uint32_t>();
        uint32_t unk = buffer.read<uint32_t>();
        if (opcode != 0x80CE5E46) {
            assert(true && "opcodes don't match, expected:" + opcode );
            return;
        }
        uint32_t header = buffer.read<uint32_t>();
        object_id = buffer.read<uint64_t>();
        uint32_t sub_opcode = buffer.read<uint32_t>();
        if (sub_opcode != T::sub_opcode()) {
            assert(true && "sub_opcodes don't match, expected:" + sub_opcode);
        }
        tick_count = buffer.read<uint32_t>();
        onDeserialize(std::move(buffer));
    }    
    uint64_t object_id;
    uint32_t tick_count;

    virtual void onSerialize(anh::ByteBuffer& buffer) const = 0;
    virtual void onDeserialize(anh::ByteBuffer buffer) = 0;
};
        
}}  // namespace swganh::base

#endif  // SWGANH_BASE_OBJECT_CONTROLLER_MESSAGE_H
