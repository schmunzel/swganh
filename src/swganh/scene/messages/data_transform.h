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
 
#ifndef SWGANH_SCENE_MESSAGES_DATA_TRANSFORM_H_
#define SWGANH_SCENE_MESSAGES_DATA_TRANSFORM_H_

#include <cstdint>
#include "swganh/base/object_controller.h"

namespace swganh {
namespace scene {
namespace messages {
    
struct DataTransform : public swganh::base::ObjectControllerMessage<DataTransform> {
    static uint16_t priority() { return 1; }
    static uint32_t opcode() { return 0x00000071; }
    float rX, rY, rZ, rW, x, y, z, speed;

    void onSerialize(anh::ByteBuffer& buffer) const 
    {
        // counter not sure where this fits in yet.
        buffer.write<uint32_t>(1);
        buffer.write(rX);
        buffer.write(rY);
        buffer.write(rZ);
        buffer.write(rW);
        buffer.write(x);
        buffer.write(y);
        buffer.write(z);
        buffer.write(speed);
    }

    void onDeserialize(anh::ByteBuffer buffer) 
    {
        int counter = buffer.read<uint32_t>();
        rX = buffer.read<float>();
        rY = buffer.read<float>();
        rZ = buffer.read<float>();
        rW = buffer.read<float>();
        x = buffer.read<float>();
        y = buffer.read<float>();
        z = buffer.read<float>();
        speed = buffer.read<float>();
    }
};

}}}  // namespace swganh:::scene::messages

#endif  // SWGANH_SCENE_MESSAGES_DATA_TRANSFORM_H_
