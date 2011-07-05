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

#ifndef SPATIAL_INDEX_REGION_H
#define SPATIAL_INDEX_REGION_H

#include <vector>
#include <glm/glm.hpp>

namespace anh
{
	namespace component
	{
		class Entity;
	};
};

namespace container_system 
{
	class region_interface
	{
	public:
		virtual bool within_extent(const glm::vec3& position) = 0;
		virtual const std::vector<glm::vec3>& verts() = 0;

		virtual void on_appear(std::shared_ptr<anh::component::Entity> object) {}
		virtual void on_enter(std::shared_ptr<anh::component::Entity> object) {}
		virtual void on_exit(std::shared_ptr<anh::component::Entity> object) {}
		virtual void on_disappear(std::shared_ptr<anh::component::Entity> object) {}
	};
};

#endif