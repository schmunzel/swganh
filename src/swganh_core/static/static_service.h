// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#pragma once

#include <map>
#include <vector>

#include "swganh/app/swganh_kernel.h"
#include "swganh/static/static_service_interface.h"
#include "swganh/static/skill_manager.h"

namespace sql
{
	class ResultSet;
}

namespace swganh
{
class EventInterface;

namespace simulation
{
	class SimulationServiceInterface;
}

namespace object
{
	class Creature;
}

namespace statics
{
	class StaticService : public swganh::statics::StaticServiceInterface
	{
	public:

		StaticService(swganh::app::SwganhKernel* kernel);
		~StaticService();

		void Startup();

		virtual swganh::service::ServiceDescription GetServiceDescription();

		std::vector<std::shared_ptr<ElevatorData>> GetElevatorDataForObject(uint64_t terminal_id);

		/*
		 * @brief Checks to see if the creature has access to the skill mod based on the given creature and it's skill levels
		 */
		virtual bool HasSkillMod(const std::shared_ptr<swganh::object::Creature>& creature, const std::string& skill_mod_name);
		/*
		 * @brief Gets a given skill mod value if exists
		 */
		virtual uint32_t GetSkillMod(const std::shared_ptr<swganh::object::Creature>& creature, const std::string& skill_mod_name);
		/*
		 * @brief Gets a given skill mod and any affected Attributes if exist
		 */
		virtual uint32_t GetTotalSkillMod(const std::shared_ptr<swganh::object::Creature>& creature, const std::string& skill_mod_name);
		/*
		 * @brief Gets All SkillMods that are applicable for this creature.
		 */
		virtual std::map<std::string, uint32_t> GetSkillMods(const std::shared_ptr<swganh::object::Creature>& creature);

		virtual std::map<std::string, uint32_t> GetSkillModTotals(const std::shared_ptr<swganh::object::Creature>& creature);

	private:

		void _loadBuildings(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadCells(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadCloneLocations(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadTerminals(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadElevatorData(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadContainers(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadTicketCollectors(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadNPCS(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);
		void _loadShuttles(swganh::simulation::SimulationServiceInterface* simulation_service, std::unique_ptr<sql::ResultSet> result,
			uint32_t scene_id, std::string scene_name);

		swganh::app::SwganhKernel* kernel_;

		std::map<uint64_t, std::vector<std::shared_ptr<ElevatorData>>> elevator_lookup_;
		SkillManager skill_mod_manager_;

	};
}
}