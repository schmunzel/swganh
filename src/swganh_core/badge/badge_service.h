// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <bitset>

#include <swganh/app/swganh_kernel.h>
#include <swganh/badge/badge_service_interface.h>
#include <swganh/connection/connection_client_interface.h>

namespace swganh {

namespace object {
	class Player;
} // namespace swganh::object

namespace equipment {
	class EquipmentService;
} // namespace swganh::equipment

namespace command {
	class CommandServiceInterface;
} // namespace swganh::command

namespace badge {

	class BadgeRegion;

	/**
	 * Badge IDs for accumulation badges.
	 */
	enum AccumulationBadges
	{
		COUNT_5 = 0,
		COUNT_10,
		COUNT_25,
		COUNT_50,
		COUNT_75,
		COUNT_100,
		COUNT_125
	};

	/**
	 * Badge IDs for exploration accumulation badges.
	 */
	enum ExpAccumulationBadges
	{
		EXP_COUNT_10 = 100,
		EXP_COUNT_20,
		EXP_COUNT_30,
		EXP_COUNT_40,
		EXP_COUNT_45
	};

	enum BadgeType
	{
		ACCUMULATION = 1,
		INTEREST,
		EXPLORATION_JEDI,
		EXPLORATION_DANGEROUS,
		EXPLORATION_EASY,
		EVENTS,
		CONTENT,
		MASTER
	};

	struct Badge
	{
		uint32_t id;
		std::string name;
		std::string sound;
		BadgeType type;

		uint32_t GetIndex() { return (uint32_t)floor((double)((id)/32)); }
		uint8_t GetBit() { return (id)%32; }
	};

	class BadgeService : public BadgeServiceInterface
	{
	public:
		BadgeService(swganh::app::SwganhKernel* kernel);
		~BadgeService();

		void Startup();

		swganh::service::ServiceDescription GetServiceDescription();

		virtual void GiveBadge(std::shared_ptr<swganh::object::Object> object, std::string name);
		virtual void GiveBadge(std::shared_ptr<swganh::object::Object> object, uint32_t id);

		virtual bool HasBadge(std::shared_ptr<swganh::object::Object> object, std::string name);
		virtual bool HasBadge(std::shared_ptr<swganh::object::Object> object, uint32_t id);

	private:
		const std::shared_ptr<Badge> FindBadge(uint32_t id);
		const std::shared_ptr<Badge> FindBadge(std::string name);

		void CheckBadgeAccumulation(std::shared_ptr<swganh::object::Object> player);
		void CheckExplorationBadgeAccumulation(std::shared_ptr<swganh::object::Object> object);
		void GiveBadge(std::shared_ptr<swganh::object::Object> object, std::shared_ptr<Badge> badge);

		std::vector<std::shared_ptr<Badge>> badges_;
		std::vector<
			std::shared_ptr<BadgeRegion>
		> badge_regions_;

		swganh::app::SwganhKernel* kernel_;
		swganh::equipment::EquipmentService* equipment_service_;
		swganh::command::CommandServiceInterface* command_service_;
	};

}} // namespace swganh::badge