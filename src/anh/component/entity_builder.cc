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

#include "entity_builder.h"

#include <glog/logging.h>
#include <boost/algorithm/string.hpp>
#include <anh/component/entity_manager.h>
#include <swganh/containers/container_component_interface.h>

namespace swganh { namespace containers {
	std::shared_ptr<NullContainerComponent> ContainerComponentInterface::NullComponent = std::make_shared<NullContainerComponent>();
	std::shared_ptr<NullContainerPermission> NullContainerComponent::null_permissions = std::make_shared<NullContainerPermission>();
}
}

namespace anh {
namespace component {

	std::map<ComponentType, std::shared_ptr<anh::component::AttributeMapperInterface<ComponentInterface>>> EntityBuilder::component_mappers_;
	std::map<EntityType, boost::property_tree::ptree> EntityBuilder::entity_templates_;
	std::map<ComponentType, ComponentCreator> EntityBuilder::component_creators_;

EntityBuilder::EntityBuilder(std::shared_ptr<EntityManager> entity_manager)
    : entity_manager_(entity_manager)
{

}

EntityBuilder::~EntityBuilder()
{
    entity_templates_.clear();
    component_mappers_.clear();
    component_creators_.clear();
    entity_tag_sets_.clear();
}

void EntityBuilder::Init(std::string object_template_dir)
{
    boost::filesystem::path templates_path(object_template_dir);
    if(!boost::filesystem::exists(templates_path))
        throw std::runtime_error("Object Template Directory Not Found!");

    LoadTemplates_(templates_path);
}

void EntityBuilder::Deinit(void)
{
}

EntityBuildErrors EntityBuilder::BuildEntity(std::uint64_t parent, const EntityType& type, const std::string& name, std::function<std::uint64_t()> id_getter)
{
	return BuildEntity(entity_manager_->GetEntity(parent), type, name, id_getter);
}

EntityBuildErrors EntityBuilder::BuildEntity(std::shared_ptr<Entity> parent, const EntityType& type, const std::string& name, std::function<std::uint64_t()> id_getter)
{
	return BuildEntity(parent, id_getter(), type, name, id_getter);
}

EntityBuildErrors EntityBuilder::BuildEntity(std::shared_ptr<Entity> parent, std::uint64_t id, const EntityType& type, const std::string& name, std::function<std::uint64_t()> id_getter)
{
    EntityBuildErrors status = BUILD_SUCCESSFUL;
    EntityTemplates::iterator i = entity_templates_.find(type);

    if(i == entity_templates_.end())
        return BUILD_FAILED;

    boost::property_tree::ptree pt = (*i).second;
    std::shared_ptr<Entity> entity(nullptr);

    // Copy Tags
    EntityTagSets::iterator ent_tag_iter = entity_tag_sets_.find(type);
    if(ent_tag_iter != entity_tag_sets_.end())
    {
        entity.reset(new Entity(id, name, ent_tag_iter->second));
    }
    else
    {
        entity.reset(new Entity(id, name));
    }


    // Build Components
    for(boost::property_tree::ptree::iterator xml_component = pt.get_child("entity").begin(); xml_component != pt.get_child("entity").end(); xml_component++) {
        if(xml_component->first.compare("component") == 0)
        {
            // Grab the component type.
            ComponentType type(xml_component->second.get<std::string>("<xmlattr>.type").c_str());

            // Search for the components creator function by the type we extracted from xml.
            ComponentCreators::iterator creators_iter = component_creators_.find(type);
            if(creators_iter == component_creators_.end()) {
                status = BUILD_INCOMPLETE;
                continue;
            }

			std::shared_ptr<ComponentInterface> component = (*creators_iter).second(id);
            component->Init(xml_component->second);
            
            // Search for a component mapper, if it exists, call it.
            ComponentAttributeMappers::iterator mapper_iter = component_mappers_.find(type);
            if(mapper_iter != component_mappers_.end())
            {
                // set db mapper for component
                component->set_attribute_mapper((*mapper_iter).second);
                // call populate to get initial values for component
                (*mapper_iter).second->Populate(component);
            }
        }
		else if(xml_component->first.compare("entity") == 0)
		{
			ComponentType type(xml_component->second.get<std::string>("<xmlattr>.type").c_str());
			if(BuildEntity(entity, type, "", id_getter) == BUILD_INCOMPLETE)
			{
				status = BUILD_INCOMPLETE;
			}
		}
    }
    // if we can't add the entity for some reason, this is a total failure.
    if (!entity_manager_->AddEntity(entity))
    {
        status = BUILD_FAILED;
    }

	if(parent != nullptr)
		parent->QueryInterface<swganh::containers::ContainerComponentInterface>("Container")->insert(nullptr, entity, true);

    return status;
}

bool EntityBuilder::RegisterAttributeMapper(const ComponentType& type, std::shared_ptr
    <anh::component::AttributeMapperInterface<ComponentInterface>> mapper)
{
    ComponentAttributeMappers::iterator i = component_mappers_.find(type);
    if(i != component_mappers_.end()) {
        return false;
    }

    component_mappers_.insert(ComponentAttributeMapperPair(type, mapper));
    return true;
}

void EntityBuilder::UnregisterAttributeMapper(const ComponentType& type)
{
    ComponentAttributeMappers::iterator i = component_mappers_.find(type);
    if(i != component_mappers_.end())
        component_mappers_.erase(i);
}

bool EntityBuilder::RegisterCreator(const ComponentType& type, ComponentCreator creator)
{
    ComponentCreators::iterator i = component_creators_.find(type);
    if(i != component_creators_.end()) {
        return false;
    }

    component_creators_.insert(ComponentCreatorPair(type, creator));
    return true;
}

void EntityBuilder::UnregisterCreator(const ComponentType& type)
{
    ComponentCreators::iterator i = component_creators_.find(type);
    if(i != component_creators_.end())
        component_creators_.erase(i);
}

bool EntityBuilder::TemplateExists(const EntityType& type)
{
    EntityTemplates::iterator i = entity_templates_.find(type);
    if(i == entity_templates_.end())
        return false;
    else
        return true;
}

bool EntityBuilder::CreatorExists(const ComponentType& type)
{
    ComponentCreators::iterator i = component_creators_.find(type);
    if(i == component_creators_.end())
        return false;
    else
        return true;
}

bool EntityBuilder::AttributeMapperExists(const ComponentType& type)
{
    ComponentAttributeMappers::iterator i = component_mappers_.find(type);
    if(i == component_mappers_.end())
        return false;
    else
        return true;
}

void EntityBuilder::LoadTemplates_(const boost::filesystem::path p)
{
    boost::filesystem::directory_iterator end_iter;
    for(boost::filesystem::directory_iterator iter(p); iter != end_iter; ++iter)
    {
        if(boost::filesystem::is_directory(iter->status()))
            LoadTemplates_(iter->path());
        else if(boost::filesystem::is_regular_file(iter->status()))
        {
            boost::property_tree::ptree pt;
            boost::property_tree::read_xml(iter->path().string(), pt);

            EntityType entity_type(pt.get<std::string>("entity.<xmlattr>.type").c_str());
            entity_templates_.insert(EntityTemplatePair(entity_type,pt));

            std::string tags = pt.get<std::string>("entity.<xmlattr>.tags", "");

            std::list<std::string> tags_list;
            boost::split(tags_list, tags, boost::is_any_of(";"));

            TagSet tags_set;
            std::for_each(tags_list.begin(), tags_list.end(), [=, &tags_set](std::string& value) {
                tags_set.insert(value);
            });

            entity_tag_sets_.insert(EntityTagSetPair(entity_type, tags_set));
        }
    }
}

} // component
} // anh