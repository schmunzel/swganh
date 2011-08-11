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
#include "transform_component.h"
#include <anh/component/entity.h>
//#include <swganh/containers/container_component_interface.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

namespace swganh {
namespace transform {

// statics
std::shared_ptr<NullTransformComponent> TransformComponentInterface::NullComponent = std::make_shared<NullTransformComponent>();

TransformComponent::TransformComponent()
: TransformComponentInterface("Anh.Transform")
{
}

TransformComponent::TransformComponent(const glm::vec3& position, const glm::quat& rotation, const float speed)
    : TransformComponentInterface("Anh.Transform")
    , position_(position)
    , rotation_(rotation)
    , speed_(speed)
{
}

void TransformComponent::Update(const float timeout)
{
}

void TransformComponent::Init(boost::property_tree::ptree& pt) 
{
    // initial default values
    position_.x = pt.get<float>("position.x", 0.0f);
    position_.y = pt.get<float>("position.y", 0.0f);
    position_.z = pt.get<float>("position.z", 0.0f);
    rotation_.x = pt.get<float>("rotation.x", 0.0f);
    rotation_.y = pt.get<float>("rotation.y", 0.0f);
    rotation_.z = pt.get<float>("rotation.z", 0.0f);
    rotation_.w = pt.get<float>("rotation.w", 1.0f);
}

void TransformComponent::rotate(const float& degrees) 
{
    // Rotate the item left by the specified degrees
	glm::quat rot;
	glm::vec3 loc;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		rotation_ = glm::rotate(rotation_, degrees, glm::vec3(0.0f, 1.0f, 0.0f));
		rot = rotation_;
		loc = position_;
	}
	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), loc, loc,  rot);
}

void TransformComponent::rotate_left(const float& degrees) 
{
    rotate(-degrees);
}

void TransformComponent::rotate_right(const float& degrees) 
{
    rotate(degrees);
}

void TransformComponent::face(const glm::vec3& target_position) 
{
    // Create a mirror direction vector for the direction we want to face.

	glm::quat rot;
	glm::vec3 loc;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		glm::vec3 direction_vector = glm::normalize(target_position - position_);
		direction_vector.x = -direction_vector.x;

		// Create a lookat matrix from the direction vector and convert it to a quaternion.
		rotation_ = glm::toQuat(glm::lookAt(direction_vector, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		// If in the 3rd quadrant the signs need to be flipped.
		if (rotation_.y <= 0.0f && rotation_.w >= 0.0f) {
			rotation_.y = -rotation_.y;
			rotation_.w = -rotation_.w;
		}

		rot = rotation_;
		loc = position_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), loc, loc, rot);
}

void TransformComponent::move(const glm::quat& rotation, float distance) 
{
    // Create a vector of the length we want pointing down the x-axis.
    glm::vec3 movement_vector(0.0f, 0.0f, distance);

    // Rotate the movement vector by the direction it should be facing.
    movement_vector = rotation * movement_vector;

    // Add the movement vector to the current position to get the new position.
	glm::vec3 old_pos;
	glm::vec3 new_pos;
	glm::quat rot;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		old_pos = position_;
		position_ += movement_vector;
		new_pos = position_;
		rot = rotation_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), old_pos, new_pos, rot);
}

void TransformComponent::move(float distance) 
{
    // Create a vector of the length we want pointing down the x-axis.
    glm::vec3 movement_vector(0.0f, 0.0f, distance);

    // Rotate the movement vector by the direction it should be facing.
	glm::vec3 old_pos;
	glm::vec3 new_pos;
	glm::quat rot;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		movement_vector = rotation_ * movement_vector;

		// Add the movement vector to the current position to get the new position.
		old_pos = position_;
		position_ += movement_vector;
		new_pos = position_;
		rot = rotation_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), old_pos, new_pos, rot);
}

void TransformComponent::move_forward(const float& distance) 
{
    move(distance);
}

void TransformComponent::move_back(const float& distance) 
{
    move(-distance);
}

float TransformComponent::rotation_angle() {
	glm::quat tmp;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		tmp = rotation_;
	}

    if (tmp.y < 0.0f && tmp.w > 0.0f) {
        tmp.y *= -1;
        tmp.w *= -1;
    }

    return glm::angle(tmp);
}

//void TransformComponent::insert_region(std::shared_ptr<swganh::regions::RegionInterface> region)
//{
//	boost::unique_lock<boost::mutex> lock(lock_);
//	regions_.insert(region);
//}

//void TransformComponent::remove_region(std::shared_ptr<swganh::regions::RegionInterface> region)
//{
//	boost::unique_lock<boost::mutex> lock(lock_);
//	regions_.erase(region);
//}
//
//std::set<std::shared_ptr<swganh::regions::RegionInterface>> TransformComponent::regions()
//{
//	boost::unique_lock<boost::mutex> lock(lock_);
//	return regions_;
//}

void TransformComponent::position(const glm::vec3& position)
{
	glm::vec3 old_pos;
	glm::quat rot;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		old_pos = position_;
		position_ = position;
		rot = rotation_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), old_pos, position, rot);
}

void TransformComponent::position(const float x, const float y, const float z)
{
	glm::vec3 old_pos;
	glm::quat rot;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		old_pos = position_;
		position_.x = x;
		position_.y = y;
		position_.z = z;
		rot = rotation_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), old_pos, glm::vec3(x, y, z), rot);
}

glm::vec3 TransformComponent::position()
{
	boost::unique_lock<boost::mutex> lock(lock_);
	return position_;
}

void TransformComponent::rotation(const glm::quat& rotation)
{
	glm::vec3 pos;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		rotation_ =  rotation;
		pos = position_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), pos, pos, rotation);
}

void TransformComponent::rotation(const float x, const float y, const float z, const float w)
{
	glm::vec3 pos;
	{
		boost::unique_lock<boost::mutex> lock(lock_);
		rotation_.x = x;
		rotation_.y = y;
		rotation_.z = z;
		rotation_.w = w;

		pos = position_;
	}

	//entity()->parent()->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->state_update(entity(), pos, pos, glm::quat(x, y, z, w));
}

glm::quat TransformComponent::rotation()
{
	boost::unique_lock<boost::mutex> lock(lock_);
	return rotation_;
}

void TransformComponent::speed(const float speed)
{
	boost::unique_lock<boost::mutex> lock(lock_);
	speed_ = speed;
}

const float TransformComponent::speed()
{
	boost::unique_lock<boost::mutex> lock(lock_);
	return speed_;
}

}
}